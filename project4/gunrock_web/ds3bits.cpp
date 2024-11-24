#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

#include "LocalFileSystem.h"
#include "Disk.h"
#include "ufs.h"

using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << argv[0] << ": diskImageFile" << endl;
    return 1;
  }

  // Parse command line arguments
  
  Disk *disk = new Disk(argv[1], UFS_BLOCK_SIZE);
  LocalFileSystem *fileSystem = new LocalFileSystem(disk);
  
  // Read the 4 KB super block starting at 0
  char buffer[4000];
  super_t super_block;
  disk -> readBlock(0, buffer);

  // Setting the size of the super block and copying the data
  memset(&super_block, 0, 4000);
  memcpy(&super_block, &buffer, 4000);

  // Read the inode bitmaps and data bitmaps
  unsigned char* inode_bitmap_ptr = new unsigned char[super_block.inode_bitmap_len];
  unsigned char* data_bitmap_ptr = new unsigned char[super_block.data_bitmap_len];
  fileSystem -> readInodeBitmap(&super_block, inode_bitmap_ptr);
  fileSystem -> readDataBitmap(&super_block, data_bitmap_ptr);

  // Print superblock metadata 
  cout << "Super" << endl;
  cout << "inode_region_addr " << super_block.inode_region_addr << endl;
  cout << "inode_region_len " << super_block.inode_region_len << endl;
  cout << "num_inodes " <<  super_block.num_inodes << endl;
  cout << "data_region_addr " << super_block.data_region_addr << endl;
  cout << "data_region_len " << super_block.data_region_len << endl;
  cout << "num_data " <<  super_block.num_data << endl << endl;
  cout << "Inode bitmap" << endl << *inode_bitmap_ptr << endl << endl; 
  cout << "Data bitmap" << endl << *data_bitmap_ptr << endl << endl; 

  return 0;
}
