#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

#include "LocalFileSystem.h"
#include "Disk.h"
#include "ufs.h"
#include <vector>

using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 3) {
    cerr << argv[0] << ": diskImageFile inodeNumber" << endl;
    return 1;
  }

  // Parse command line arguments
  
  Disk *disk = new Disk(argv[1], UFS_BLOCK_SIZE);
  LocalFileSystem *fileSystem = new LocalFileSystem(disk);
  int inodeNumber = stoi(argv[2]);

  if(inodeNumber < 0 ){
    cerr << "Error reading file" << endl;
    exit(1);
  }

  // build the inode
  inode_t* my_inode = fileSystem->buildInode(inodeNumber); 
  
  // if the size of the file is 0 exit
  if(my_inode->size == 0 || my_inode->type == UFS_DIRECTORY){
    cerr << "Error reading file" << endl;
    exit(1);
  }

  // Read the data from i - num
  char* buffer = new char[my_inode->size + 1];
  buffer[my_inode->size] = '\0';

  fileSystem -> read(inodeNumber, buffer, my_inode->size);
  vector<unsigned int> validDataBlocks = fileSystem->getValidDataBlocks(my_inode);

  // Print out cat message
  cout << "File blocks" << endl;

  for(unsigned int block: validDataBlocks){
    cout << block << endl;
  }

  cout << endl;

  cout << "File data" << endl << buffer;
  

  // Free memory
  free(my_inode);
  delete[] buffer;
  delete(fileSystem);
  delete(disk);
  return 0;
}
