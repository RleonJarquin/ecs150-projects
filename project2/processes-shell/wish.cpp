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
		vector<string> tokenize(string my_line){
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
					cout << "ERROR: unable to open file" << endl;
					exit(1);
				}

				while(getline(my_file, my_line)){
					vector<string> my_tokens = tokenize(my_line);
					all_commands.push_back(my_tokens);
				}

				for(vector<string> my_command : all_commands){
					execute_command(my_command);
				}
				my_file.close();
				return;
		}

		void execute_command(vector<string> command){
			int pid = fork();

			if(pid < 0){
				cout << "ERROR: fork failed." << endl;
				exit(1);
			}
			else if(pid == 0){
				string path = "/bin/" + command[0];
				char *exec_path = strdup(path.c_str());				
				char **args = new char*[command.size() + 1];

				// Insert the string into the args array 
				for(size_t i = 0; i < command.size(); i++){
					args[i] = strdup(command[i].c_str());
				}

				args[command.size()] = NULL;
				execv(exec_path, args);


				// Still got to free the allocated args array
				free(exec_path);
				for (size_t i = 0; i < command.size(); i++) {
        			free(args[i]); // Free each argument
    			}
				delete(args); 

				exit(0);
			}
			else{
				wait(NULL);
			}
			return;
		} 

		

	public: 
		wish(int argc, char* argv[]){
			// Check if you can open the file the file for batch mode
			if(argc == 1){
				string my_line;	
				// Enter interactive mode
				while(true){
					cout << "wish> ";
					getline(cin, my_line);
					vector<string> my_tokens = tokenize(my_line);
					
					if(my_tokens[0] == "exit" && my_tokens.size() == 1){
						exit(0);
					}
					else if(my_tokens[0] == "exit"){
						continue;
					}
					else{
						execute_command(my_tokens);
					}
				}
			}
			else if(argc == 2){
				execute_batch(argv[1]);
			}
		}

};

int main(int argc, char* argv[]){

	wish(argc, argv);
	exit(0);

}

