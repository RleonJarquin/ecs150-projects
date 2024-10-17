#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>


using namespace std;

class wish {
	private:
		vector<string> my_paths;

		bool char_in_vector(vector<string> &command, const char my_char){
			// // Check if parallel is applicable. If so extract the command
			// auto it = find(command.begin(), command.end(), my_char);

			// // Check if & is in my_line
			// if(it != command.end()){
			// 	return true;
			// }

			// Check if the character is a substring of the elements
			for(string substring : command){
				if(substring.find(my_char) != string::npos){
					return true;
				}
			}
			return false;
		}

		void make_parsable(vector<string> &command, const char my_char){	
			// For the given character, insert white space and reparse the modified command vector
			vector<string> new_command;
			const char* curr_token;
			char* new_token;
			string new_line = "";

			for(string token : command){
				curr_token = token.c_str();
				for(int i = 0; curr_token[i] != '\0'; i++){
					if(curr_token[i] == my_char){
						new_line += ' ';
						new_line += my_char;
						new_line += ' ';

					}
					else{
						new_line += curr_token[i];
					}
				}
				new_line += " ";
			}

			new_line += '\0';
			char* mutable_line = new char[new_line.length() - 1];
			strcpy(mutable_line, new_line.c_str());
			new_token = strtok(mutable_line, " ");

			while(new_token != nullptr){
				new_command.push_back(string(new_token));
				new_token = strtok(nullptr," ");
			}
			
			command = new_command;
			delete(mutable_line);
		}

		void parse_parallel(vector<vector<string>> &all_commands, vector<string> &command){
			make_parsable(command, '&');
			vector<string> sub_command;

			// Loop through command and add subcommand to all commands
			for(size_t i = 0; i < command.size(); i++){
				if(command[i] == "&"){
					// Check if parallel syntax has been violated otherwise add sub command to all commands
					if(command.size() == 1){
						all_commands.push_back({});
						return;
					}
					else if(i == 0 ){// || (i == command.size() - 1 && command[i] == "&")){
						continue;
					}
					else if((i - 1 < command.size() && i + 1 < command.size()) &&  
					(command[i - 1] == "&" || command[i + 1] == "&")){
						all_commands = {};
						return;
					}
					else{
						all_commands.push_back(sub_command);
						sub_command = {};
					}
				}
				else{
					sub_command.push_back(command[i]);
				}
			}

			if(sub_command.size() != 0){
				all_commands.push_back(sub_command);
			}
		}
	
		void parse_redirection(vector<string> &command){
			make_parsable(command, '>');
			vector<string> sub_command;
			string path;
			int max_redirection = 1;
			 
			 // Only accepts 1 redirection 
			for(size_t i = 0; i < command.size(); i++){
				if(command[i] == ">"){
					// Check if parallel syntax has been violated otherwise add sub command to all commands
					if(i + 1 >= command.size() || i - 1 > command.size() || i + 1 > command.size() || i + 1 != command.size() - 1
					|| max_redirection < 0|| command[i - 1] == ">" || command[i + 1] == ">"){
						command = {"__INVALID__"};
						return;
					}
					else{
						command = sub_command;
						path = command[i + 1];
						max_redirection--;
					}
				}
				else{
					sub_command.push_back(command[i]);
				}
			}

			// Open the and redirect STDOUT and STERROR to the file
			int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | 0644);

			// If file did not open return no commands and let execute command print errors
			if(fd < 0){
				command = {"__INVALID__"};
				return;
			}

			if(dup2(fd, STDOUT_FILENO) == -1){
				command = {"__INVALID__"};
				close(fd);
				return;
			};

			if(dup2(fd, STDERR_FILENO) == -1){
				command = {"__INVALID__"};
				close(fd);
				return;
			}

			close(fd);
		}

		void print_error(){
			    char error_message[30] = "An error has occurred\n";
				write(STDERR_FILENO, error_message, strlen(error_message)); 
		}

		void my_exit(vector<string> &my_line){
			if(my_line[0] == "exit" && my_line.size() == 1){
				exit(0);
			}
			else{
				print_error();
			}
		}

		void my_cd(vector<string> &my_line){ 
			if(my_line.size() == 1 || my_line.size() > 2){
				print_error();
			}
			else{
				const char* path = my_line[1].c_str();
				chdir(path);
			}
		}

		void my_path(vector<string> &my_line){ 
			// If there are 0 args. Set the paths to empty string
			if(my_line.size() == 1){
				my_paths = {};
			}
			// Otherwise set the paths 
			else{
				vector<string> curr_paths;
				for(size_t i = 1; i < my_line.size(); i++){
					curr_paths.push_back(my_line[i]);
				}

				my_paths = curr_paths;

			}
		}
		
		void process_command(vector<string> &my_line){
			// Either execute built in commands or regular commands

			// Parse for parallel commands 
			vector<vector<string>> all_commands;

			// Check if & is in my_line
			if(char_in_vector(my_line, '&')){
				parse_parallel(all_commands, my_line);
			}
			else{
				all_commands = {my_line};
			}
			

			
			if(my_line[0] == "exit"){
				my_exit(my_line);
			}
			else if(my_line[0] == "cd"){
				my_cd(my_line);
			}
			else if(my_line[0] == "path"){
				my_path(my_line);
			}
			else if(my_paths.size() == 0 || all_commands.size() == 0){
				print_error();
			}
			else if(all_commands[0].size() == 0){
				return;
			}
			else if(all_commands.size() > 1){
				for(vector<string> my_command : all_commands){
					process_command(my_command);
				}
			}
			else{
				execute_command(all_commands[0]);
			}
		} 
		
		vector<string> tokenize(string &my_line){
			vector<string> my_tokens;
			istringstream stream(my_line);
			string my_word;
			
			// Tokenize my string
			while(stream >> my_word){
				my_tokens.push_back(my_word);
			}

			return my_tokens;
		}

		void execute_batch(char* file){
				ifstream my_file(file);
				vector<vector<string>> all_commands;
				string my_line;
				if(!my_file){
					print_error();
					exit(1);
				}

				while(getline(my_file, my_line)){
					vector<string> my_tokens = tokenize(my_line);
					if(my_tokens.size() != 0){
						all_commands.push_back(my_tokens);
					}
				}

				for(vector<string> my_command : all_commands){
					process_command(my_command);
				}
				my_file.close();
		}

		void execute_command(vector<string> command){
			int pid = fork();
			// int pid = 0;

			if(pid < 0){
				print_error();
			}
			else if(pid == 0){
				// Check if accessing the executable is a valid option
				string path;
				bool path_accessible;

				// Check if > is in command
				if(char_in_vector(command, '>')){
					parse_redirection(command);
				}
				
				for(string curr_path : my_paths){
					path = curr_path + "/" + command[0];
					if((path_accessible = access(path.c_str(), X_OK) == 0)){
						break;
					}
				}

				if(path_accessible){
					char *exec_path = strdup(path.c_str());				
					char **args = new char*[command.size() + 1];

					// Insert the string into the args array 
					for(size_t i = 0; i < command.size(); i++){
						args[i] = strdup(command[i].c_str());
					}

					args[command.size()] = NULL;
					execv(exec_path, args);


					// Still got to free the allocated args array
					for (size_t i = 0; i < command.size(); i++) {
						free(args[i]); // Free each argument
					}
					
					free(exec_path);
					delete(args); 
				}
				else{
					print_error();
				}
				
				exit(0);
			}
			else{
				wait(NULL);
			}
		} 

		

	public: 
		wish(int argc, char* argv[]){
			// Initializes all paths
			my_paths.push_back("/bin");

			// Check if you can open the file the file for batch mode
			if(argc == 1){
				string my_line;	
				// Enter interactive mode
				while(true){
					cout << "wish> ";
					getline(cin, my_line);
					vector<string> my_tokens = tokenize(my_line);
					process_command(my_tokens);
				}
			}
			else if(argc == 2){
				execute_batch(argv[1]);
			}
			else{
				print_error();
				exit(1);
			}
		}

};

int main(int argc, char* argv[]){
	wish(argc, argv);
	exit(0);
}