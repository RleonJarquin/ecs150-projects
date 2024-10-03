#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]){

	if(argc != 2){
		cerr << "Error: Invalid number of arguments." << endl;
		cerr << "Usage: " << argv[0] << " <file>" << endl;
		return 1;		 
	}
	// Change this to accept multpile files. 	
	int fileDescriptor = open(argv[1], O_RDONLY);
	char* buff[4096];
	int bytesRead;

	if(fileDescriptor < 0){
		cout << "cannot open file" << endl;
		return 1;
	}
	
	while ((bytesRead = read(fileDescriptor, buff, sizeof(buff)-1)) > 0){
		write(1, buff, bytesRead);
	}	

	close(fileDescriptor);
	return 0;
}
