#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <bitset>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

void decode_substring(char my_letter, int len, string& my_decoding){
	for(int i = 0; i < len; i++){
		my_decoding += my_letter;
	}
	return;
}


void decode(const int fd, string& my_decoding){
	char my_encoding[5];
	char curr_letter = '\0';
	int curr_int;
	int curr_idx = 0;
	vector<char> my_char;
	vector<int> my_char_len;
	
	while(read(fd, my_encoding, sizeof(int) + sizeof(char))){
		// Extract the letter and the integer
		curr_letter = my_encoding[sizeof(int)];
		memcpy(&curr_int, my_encoding, sizeof(int));

		// Store letter and integer in vectors and update the current index.
		my_char.push_back(curr_letter);
		my_char_len.push_back(curr_int);	
		curr_idx++;	
	}

	// Loop through each of the vectors and encode the values
	for(size_t i = 0; i < my_char.size(); i++){
		decode_substring(my_char[i], my_char_len[i], my_decoding);
	}

	return;
}

int main(int argc, char* argv[]){
	if(argc < 2){
		cout << "wunzip: file1 [file2 ...]" << endl;
		exit(1);
	}

	string my_decoding = "";

	// First pass does the encoding
	for(int file = 1; file < argc; file++){
		int fd = open(argv[file], O_RDONLY);

		if(fd < 0){
			cout << "wunzip: cannot open file" << endl;
			exit(1);
		}

		// Encode the file
		decode(fd, my_decoding);
		close(fd);
	}

	// Write the zipped file to STDIN_FILENO
	write(STDOUT_FILENO, my_decoding.c_str(), my_decoding.length());
	exit(0);
}
