#include <iostream>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;



void find_pattern(const int patternSize, const char* pattern, const int fileDescriptor){
	
	int j = 0;
	char buff[2]; 
	string my_line = "";
	const char null_char = '\0';
	const char nl_char = '\n';
	bool match = false;
	buff[1] = null_char; 

	// Process 4096 characters 
	while (read(fileDescriptor, buff, 1) > 0){
		// If there is a pattern match and the current character is not \n continue
		my_line += buff[0];
		if(match && buff[0] != nl_char){
			continue;
		}
		else{
			// Otherwise if the character is \n and the pattern matches write to STDOUT and reset
			if(buff[0] == nl_char){
				if(match){
					const char* my_line_ptr = my_line.c_str(); 
					write(1, my_line_ptr, my_line.length());
				}
				my_line = "";
				match = false;
				j = 0;
				continue;
			}
			// Else check if buff[i] matches pattern[j]
			if(buff[0] == pattern[j]){
				j++;
				if(patternSize == j){
					match = true;
				}	
			}
			else{
				j = 0;
			}
		}
		
	}
	
	if(fileDescriptor != 0){
		close(fileDescriptor);
	}
		return; 
}

	
int main(int argc, char* argv[]){

	if(argc == 1){
		cout << "wgrep: searchterm [file ...]" << endl;
		exit(1);
	}

	int patternSize = strlen(argv[1]);
	char* pattern = argv[1];
	bool file_passed;
	int fileDescriptor;

	if(argc == 2){
		file_passed = false;
	}
	else{
		file_passed = true;
	}
	
	if(file_passed){
		for(int file = 2; file < argc;file++){
			fileDescriptor = open(argv[file], O_RDONLY);

			if(fileDescriptor < 0){
				cout << "wgrep: cannot open file" << endl;
				exit(1);
			}

			find_pattern(patternSize, pattern, fileDescriptor);
		}
	}
	else{
		fileDescriptor = STDIN_FILENO;
		find_pattern(patternSize, pattern, fileDescriptor);
	}

        return 0;
}
