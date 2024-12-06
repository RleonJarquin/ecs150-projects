#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

#include "StringUtils.h"
#include "LocalFileSystem.h"
#include "Disk.h"
#include "ufs.h"

using namespace std;


// Use this function with std::sort for directory entries
bool compareByName(const dir_ent_t& a, const dir_ent_t& b) {
    return std::strcmp(a.name, b.name) < 0;
}


int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << argv[0] << ": diskImageFile directory" << endl;
    cerr << "For example:" << endl;
    cerr << "    $ " << argv[0] << " tests/disk_images/a.img /a/b" << endl;
    return 1;
  }

  // parse command line arguments 
  Disk *disk = new Disk(argv[1], UFS_BLOCK_SIZE);
  LocalFileSystem *fileSystem = new LocalFileSystem(disk);
  string directory = string(argv[2]); 

  // Basic variable declarations
  char buffer[4096];
 
  // Iteratively lookup the file directory starting from root
  int curr_inode_number = 0;
  int last_inode_number;
  bool is_file = false;
  char* last_token;

  char* new_token = strtok(const_cast<char*>(directory.c_str()), ("/"));

  while(new_token != NULL){
      last_inode_number = curr_inode_number;
      string my_entry = new_token;
      curr_inode_number = fileSystem -> lookup(curr_inode_number, my_entry);
      last_token = new_token;
      new_token = strtok(NULL, "/");
  }

  // Initializing data inode
  inode_t* my_inode = fileSystem->buildInode(curr_inode_number);

  if(my_inode->type == UFS_REGULAR_FILE){
    inode_t* new_inode = fileSystem->buildInode(last_inode_number);
    memcpy(my_inode, new_inode, sizeof(inode_t));
    free(new_inode);
    // my_inode = fileSystem->buildInode(last_inode_number);
    curr_inode_number = last_inode_number;
    is_file = true;
  }

  // Initializing dir_entry_arry
  dir_ent_t* dir_entry_arr = new dir_ent_t[my_inode->size / sizeof(dir_ent_t)];
 
  // Read from the dir_entry 
  fileSystem -> read(curr_inode_number, buffer, UFS_BLOCK_SIZE);
  memcpy(dir_entry_arr, buffer, my_inode->size);

  // turn array into vector 
  vector<dir_ent_t> dir_entry_vec;
  int num_entries = my_inode->size / sizeof(dir_ent_t);

  for(int i = 0; i < num_entries; i++){
    dir_entry_vec.push_back(dir_entry_arr[i]);
  }

  sort(dir_entry_vec.begin(), dir_entry_vec.end(), compareByName);

  // If it is a file print out the entry data otherwise print out the directory 
  if(is_file){
    for(dir_ent_t entry: dir_entry_vec){
      if(strcmp(entry.name, last_token) == 0 ){
          cout << entry.inum << "\t" << entry.name << endl;
      }
    }
  }
  else{
      for(dir_ent_t entry: dir_entry_vec){
        cout << entry.inum << "\t" << entry.name << endl;
      }
  }
  
  // free memory
  free(my_inode);
  delete[] dir_entry_arr;
  delete fileSystem;
  delete disk;
  return 0;
}
