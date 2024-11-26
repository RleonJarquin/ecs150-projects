#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "LocalFileSystem.h"
#include "ufs.h"

#include <string.h>

using namespace std;


LocalFileSystem::LocalFileSystem(Disk *disk) {
  this->disk = disk;
}

void LocalFileSystem::readSuperBlock(super_t *super) {
    disk -> readBlock(0, super);
}

void LocalFileSystem::readInodeBitmap(super_t *super, unsigned char *inodeBitmap) {

}

void LocalFileSystem::writeInodeBitmap(super_t *super, unsigned char *inodeBitmap) {

}

void LocalFileSystem::readDataBitmap(super_t *super, unsigned char *dataBitmap) {

}

void LocalFileSystem::writeDataBitmap(super_t *super, unsigned char *dataBitmap) {

}

void LocalFileSystem::readInodeRegion(super_t *super, inode_t *inodes) {
  int inodes_addr = super -> inode_region_addr;
  int inodes_len = super -> inode_region_len;
  // constant size exists already 
  char buffer[UFS_BLOCK_SIZE];
  string inodes_blocks;
  
  for(int i = 0; i < inodes_len; i++){
      // Read the block into the buffer
      disk -> readBlock(inodes_addr + i, buffer);
      inodes_blocks += buffer;
  }

  // copy the blocks into the inodes data structure
  memcpy(inodes, &inodes_blocks, inodes_len * UFS_BLOCK_SIZE);
}

void LocalFileSystem::writeInodeRegion(super_t *super, inode_t *inodes) {

}

int LocalFileSystem::lookup(int parentInodeNumber, string name) {
  // read the parentInodeNumber into inode struct
  inode_t* inode;
  stat(parentInodeNumber, inode);


  // declare buffers 
  int size = inode -> size;
  char* my_dir_entries = new char[size];
  char buffer[UFS_BLOCK_SIZE];


  // read the direct ptr of inode to get dir_ent_t
  for(unsigned int direct_ptr: inode -> direct){
    // if all the entries have been read break 
    if(size == 0){
      break;
    }

    // read the block, bytesread, and offset for dir_entry arr
    disk -> readBlock(direct_ptr, buffer);
    int bytes_read = strlen(buffer);
    int offset = strlen(my_dir_entries);

    // read the data from the buffer into the arr
    memcpy(my_dir_entries + offset, buffer, bytes_read);
    size -= bytes_read;
  }

  // set the size of the array and copy bytes
  dir_ent_t dir_entry_arr[sizeof(my_dir_entries)];
  mempcpy(dir_entry_arr, my_dir_entries, sizeof(my_dir_entries));

  // extract the indoe from the array and return it
  for(dir_ent_t entry: dir_entry_arr){
    if(name.c_str() == entry.name){
      return entry.inum;
    }
  }

  return ENOTFOUND;
}

int LocalFileSystem::stat(int inodeNumber, inode_t *inode) {
  // read the super block
  super_t* super;
  readSuperBlock(super);

  // setting the data for inodes 
  int num_inodes = super -> num_inodes;
  inode_t* inodes = new inode_t[num_inodes * num_inodes];

  // Reading the inodes block
  readInodeRegion(super, inodes);

  // Read the inode of the inode from the inodes array
  *inode = inodes[inodeNumber];
  return 0;
}

int LocalFileSystem::read(int inodeNumber, void *buffer, int size) {
  return 0;
}

int LocalFileSystem::create(int parentInodeNumber, int type, string name) {
  return 0;
}

int LocalFileSystem::write(int inodeNumber, const void *buffer, int size) {
  return 0;
}

int LocalFileSystem::unlink(int parentInodeNumber, string name) {
  return 0;
}

