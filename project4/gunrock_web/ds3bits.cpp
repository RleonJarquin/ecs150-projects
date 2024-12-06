#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

#include "LocalFileSystem.h"
#include "Disk.h"
#include "ufs.h"

#include <stdlib.h>


using namespace std;


int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << argv[0] << ": diskImageFile" << endl;
    return 1;
  }

  // Read the 4 KB super block starting at 0
  // Parse command line arguments
  Disk *disk = new Disk(argv[1], UFS_BLOCK_SIZE);
  LocalFileSystem *fileSystem = new LocalFileSystem(disk);

  super_t* super = fileSystem->buildSuper();
  fileSystem->readSuperBlock(super);

  // memset(&super_block, 0, 4000);
  // fileSystem -> readSuperBlock(super);

  // Read the inode bitmaps and data bitmaps
  unsigned char* inode_bitmap_ptr = new unsigned char[super->inode_bitmap_len * UFS_BLOCK_SIZE];
  unsigned char* data_bitmap_ptr = new unsigned char[super->data_bitmap_len * UFS_BLOCK_SIZE];
  fileSystem -> readInodeBitmap(super, inode_bitmap_ptr);
  fileSystem -> readDataBitmap(super, data_bitmap_ptr);

  // Print superblock metadata 
  cout << "Super" << endl;
  cout << "inode_region_addr " << super->inode_region_addr << endl;
  cout << "inode_region_len " << super->inode_region_len << endl;
  cout << "num_inodes " <<  super->num_inodes << endl;
  cout << "data_region_addr " << super->data_region_addr << endl;
  cout << "data_region_len " << super->data_region_len << endl;
  cout << "num_data " <<  super->num_data << endl << endl;


  cout << "Inode bitmap" << endl;
  size_t num_bytes_inode = ((super->num_inodes + 7) / 8);
  
  for(size_t i = 0; i < num_bytes_inode; i++){
    unsigned char byte = inode_bitmap_ptr[i];
    cout << (int)byte << " ";
  }
  
  cout << endl << endl; 

  cout << "Data bitmap" << endl;
  size_t num_bytes_data = ((super->num_data + 7) / 8);

  for(size_t i = 0; i < num_bytes_data; i++){
    unsigned char byte = data_bitmap_ptr[i];
    cout << (int)byte << " ";
  }

  cout << endl; 

  free(super);
  delete[] inode_bitmap_ptr;
  delete[] data_bitmap_ptr;


  delete fileSystem;
  delete disk;

  return 0;
}
