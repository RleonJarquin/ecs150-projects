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


using namespace std;

class wish {
	private:
		vector<string> my_paths;

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

			}
		}
		
		void process_command(vector<string> &my_line){
			// Either execute built in commands or regular commands
			if(my_line[0] == "exit"){
				my_exit(my_line);
			}
			else if(my_line[0] == "cd"){
				my_cd(my_line);
			}
			else if(my_line[0] == "path"){
				my_path(my_line);
			}
			else if(my_paths.size() == 0){
				print_error();
			}
			else{
				execute_command(my_line);
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
					all_commands.push_back(my_tokens);
				}

				for(vector<string> my_command : all_commands){
					process_command(my_command);
				}
				my_file.close();
		}

		void execute_command(vector<string> command){
			int pid = fork();

			if(pid < 0){
				print_error();
			}
			else if(pid == 0){
				// Check if accessing the executable is a valid option
				string path;
				bool path_accessible;
				
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

