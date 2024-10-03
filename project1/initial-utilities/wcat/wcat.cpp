#include <iostream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[]){
	
	for(int file = 1; file < argc;file++){
		// Change this to accept multpile files. 	
		int fileDescriptor = open(argv[file], O_RDONLY);
		char* buff[4096];
		int bytesRead;

		if(fileDescriptor < 0){
			cout << "wcat: cannot open file" << endl;
			return 1;
		}	
	
		while ((bytesRead = read(fileDescriptor, buff, sizeof(buff)-1)) > 0){
			write(1, buff, bytesRead);
		}	

		close(fileDescriptor);	
	}
	return 0;
}
