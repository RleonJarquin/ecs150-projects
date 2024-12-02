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
  string my_dir_entries;


  // Iteratively lookup the file directory starting from root
  int curr_dir_entry = 0;

  while(char* new_token = strtok(const_cast<char*>(directory.c_str()), ("/"))){
      string my_entry = new_token;
      curr_dir_entry = fileSystem -> lookup(curr_dir_entry, my_entry);
  }
 
  // Read from the dir dir_entry 
  while(fileSystem -> read(curr_dir_entry, buffer, UFS_BLOCK_SIZE)){
    my_dir_entries += buffer;
  }

  // memcpy my_dir_entrys into arr of dir_ent_t
  dir_ent_t dir_entry_arr[sizeof(my_dir_entries)]; 
  memcpy(&dir_entry_arr, &my_dir_entries, sizeof(my_dir_entries));

  // turn array into vector 
  vector<dir_ent_t> dir_entry_vec;
  for(dir_ent_t dir_entry: dir_entry_arr){
    dir_entry_vec.push_back(dir_entry);
  }

  // Sort the array 
  // sort(dir_entry_vec.begin(), dir_entry_vec.end(), [](dir_ent_t a, dir_ent_t b){
  //   return a.inum < b.inum;
  // });
  sort(dir_entry_vec.begin(), dir_entry_vec.end(), compareByName);

  for(dir_ent_t entry: dir_entry_vec){
    cout << entry.inum << "\t" << entry.name << endl;
  }

  // free memory
  delete fileSystem;
  delete disk;

  return 0;
}
