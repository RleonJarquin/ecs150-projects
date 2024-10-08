#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <bitset>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <cstdint>
using namespace std;


void clean_string(string& my_encoding){
	// inputs
	istringstream stream(my_encoding);
	int32_t curr_int;
	int curr_idx = -1;
	char clean_int[sizeof(int)];
	char curr_letter = '\0';
	char buff[5];
	char temp[4];
	string clean_encoding;
	vector<char> my_char;
	vector<int32_t> my_char_len;

	// Processing
	while(stream.read(buff, 5)){
		// If the first char is '\n' shift the 4 msb to the lower 4 bytes and add new line to string
		if(buff[0] == '\n'){
			memcpy(temp, buff + 1, sizeof(int));
			memcpy(buff, temp, sizeof(int));
			stream.read(temp, 1);
			buff[sizeof(int)] = temp[0]; 
			
		}

		// Check if the curr letter is different from buff
		if(curr_letter != buff[4]){
			// If it is different append curr_int and curr letter and reset values
			curr_letter = buff[4];
			memcpy(&curr_int, buff, sizeof(int));
			my_char.push_back(curr_letter);
			my_char_len.push_back(curr_int);
			curr_idx++;
		}
		else{
			// If it is the same letter. Then update the integer. 	
			memcpy(&curr_int, buff, sizeof(int));
			my_char_len[curr_idx] += curr_int; 
		}
	}


	// Loop through each of the vectors and encode the values
	for(size_t i = 0; i < my_char.size(); i++){
		curr_int = my_char_len[curr_idx];
		memcpy(clean_int, &curr_int, sizeof(int));
		clean_encoding.append(clean_int, sizeof(int));
		clean_encoding += curr_letter;
	}

	my_encoding = clean_encoding;
	return;
}

void encode_substring(char my_letter, int len, string& my_encoding){
	if(my_letter != '\n'){
		// char* subencoding = new char[sizeof(int) + 2];
		char subencoding[sizeof(int)];
		memcpy(subencoding, &len, sizeof(int));
		// memcpy(subencoding + sizeof(int), &my_letter, sizeof(char));
		// subencoding[sizeof(int) + 1] = '\0';
		my_encoding.append(subencoding, sizeof(int));
		my_encoding += my_letter;
	}
	else{
		char subencoding[sizeof(int)];
		memcpy(subencoding, &len, sizeof(int));
		my_encoding.append(subencoding, sizeof(int));
		my_encoding += '\n';
	}
	return;
}


void encode(const int fd, string& my_encoding){
	char buff[2] = {' ', '\0'};
	char curr_letter = '\0';
	size_t curr_idx = 0;
	vector<char> my_char;
	vector<int> my_char_len;
	
	while(read(fd, buff, 1)){
		if(buff[0] != '\n'){
			// If we have a new letter, add it the the vectors and update the current index.
			if(curr_letter != buff[0]){		
				curr_letter = buff[0];
				my_char.push_back(curr_letter);
				curr_idx = my_char.size() - 1;
				my_char_len.push_back(1);
			}
			else{
				my_char_len[curr_idx] = my_char_len[curr_idx] + 1;	
			}
		}
		else{
			// If there is a new line in the array insert in the vector and continue 
			my_char.push_back('\n');
			my_char_len.push_back(1);
			continue;
		}
	}

	// Loop through each of the vectors and encode the values
	for(size_t i = 0; i < my_char.size(); i++){
		encode_substring(my_char[i], my_char_len[i], my_encoding);
	}

	return;
}

int main(int argc, char* argv[]){
	if(argc < 2){
		cout << "wzip: file1 [file2 ...]" << endl;
		exit(1);
	}

	string my_encoding = "";

	// First pass does the encoding
	for(int file = 1; file < argc; file++){
		int fd = open(argv[file], O_RDONLY);

		if(fd < 0){
			cout << "wzip: cannot open file" << endl;
			exit(1);
		}

		// Encode the file
		encode(fd, my_encoding);

		close(fd);
	}
	// Write the zipped file to STDIN_FILENO
	if(argc != 2){
		clean_string(my_encoding);
	}
	write(STDOUT_FILENO, my_encoding.c_str(), my_encoding.length());
	exit(0);
}
