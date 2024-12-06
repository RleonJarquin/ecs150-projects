#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "LocalFileSystem.h"
#include "ufs.h"

#include <string.h>

#include <stdlib.h>

using namespace std;


LocalFileSystem::LocalFileSystem(Disk *disk) {
  this->disk = disk;
}

void LocalFileSystem::readSuperBlock(super_t *super) {
    disk->readBlock(UFS_ROOT_DIRECTORY_INODE_NUMBER, super);
}

void LocalFileSystem::readInodeBitmap(super_t *super, unsigned char *inodeBitmap) {  
  // For the length of the bitmap read bytes into inodeBitmap
  int len = super->inode_bitmap_len;

  for(int i = 0; i < len; i++){
    int start = super->inode_bitmap_addr;
    disk->readBlock(start + i, (char*)inodeBitmap + (i * UFS_BLOCK_SIZE));    
  }
}

void LocalFileSystem::writeInodeBitmap(super_t *super, unsigned char *inodeBitmap) {

}

void LocalFileSystem::readDataBitmap(super_t *super, unsigned char *dataBitmap) {
  // For the length of the bitmap read bytes into dataBitmap
  int len = super->data_bitmap_len;
  
  for(int i = 0; i < len; i++){
    // read the curr block of the curr bitmap
    int start = super->data_bitmap_addr;
    disk->readBlock(start + i, (char*)dataBitmap + (i * UFS_BLOCK_SIZE));    
  }
}

void LocalFileSystem::writeDataBitmap(super_t *super, unsigned char *dataBitmap) {

}

void LocalFileSystem::readInodeRegion(super_t *super, inode_t *inodes) {
  int inodes_addr = super->inode_region_addr;
  int inodes_len = super->inode_region_len;
  
  for(int i = 0; i < inodes_len; i++){
      // read inode with offset
      disk->readBlock(inodes_addr + i, reinterpret_cast<char*>(inodes) + (i * UFS_BLOCK_SIZE));
  }
}

void LocalFileSystem::writeInodeRegion(super_t *super, inode_t *inodes) {

}

int LocalFileSystem::lookup(int parentInodeNumber, string name) {
  // read the parentInodeNumber into inode struct
  inode_t* my_inode = buildInode(parentInodeNumber);
 
  // declare data structures 
  int size = my_inode->size;
  dir_ent_t* dir_entry_arr = new dir_ent_t[my_inode->size / sizeof(dir_ent_t)]; 
  char buffer[UFS_BLOCK_SIZE];

  // read from the buffer and copy to the array 
  read(parentInodeNumber, buffer, UFS_BLOCK_SIZE);
  memcpy(dir_entry_arr, buffer, size);

  // extract the indoe from the array and return it
  int num_entries = my_inode->size / sizeof(dir_ent_t);

  for(int i = 0; i < num_entries; i++){
    // if the entry name matches for the string name return inum
    dir_ent_t my_entry = dir_entry_arr[i];
    if(strcmp(name.c_str(), my_entry.name) == 0){
      free(my_inode);
      delete[] dir_entry_arr;
      return my_entry.inum;
    }
  }

  cerr << "Directory not found" << endl;
  exit(1);
}

int LocalFileSystem::stat(int inodeNumber, inode_t *inode) {
  // read the super block
  super_t* super = buildSuper();

  // setting the data for inodes 
  int num_inodes = super->num_inodes;
  inode_t* inodes = (inode_t*)malloc(num_inodes * sizeof(inode_t));  // Allocate with malloc

  // Reading the inodes block
  readInodeRegion(super, inodes);

  // Read the inode of the inode from the inodes array
  inode_t my_inode = inodes[inodeNumber];
  memcpy(inode, &my_inode, sizeof(my_inode));

  // free memory
  free(super);
  free(inodes);  
  return 0;
}

int LocalFileSystem::read(int inodeNumber, void *buffer, int size) {
  // Read the inode
  inode_t* my_inode = buildInode(inodeNumber);
  
  // Get the valid data blocks
  vector<unsigned int> valid_data_blocks = getValidDataBlocks(my_inode);

  // Initial Parameters
  char buff[UFS_BLOCK_SIZE];
  int bytes_left = my_inode->size;
  int total_bytes_read = 0;

  for(unsigned int block: valid_data_blocks){
    if(bytes_left == 0){
      break;
    }
    disk->readBlock(block, buff);
    
    int bytes_to_copy = min(bytes_left, UFS_BLOCK_SIZE);
    memcpy((char*)buffer + total_bytes_read, buff, bytes_to_copy);

    // read the data from the buffer into the arr
    bytes_left -= bytes_to_copy;
    total_bytes_read += bytes_to_copy;
  }

  // Free memory 
  free(my_inode);
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

// Rubens helper functions
super_t* LocalFileSystem::buildSuper(){
  super_t* super_ptr = (super_t*)malloc(UFS_BLOCK_SIZE);
  readSuperBlock(super_ptr);
  return super_ptr;
}

inode_t* LocalFileSystem::buildInode(int inodeNumber){
  inode_t* my_inode = (inode_t*)malloc(sizeof(inode_t));
  stat(inodeNumber, my_inode);
  return my_inode;
}

vector<unsigned> LocalFileSystem::getValidDataBlocks(inode_t* my_inode){
  // read the super block
  super_t* super = buildSuper();

  // Allocate space for the bitmap
  unsigned char* data_bitmap =  (unsigned char*)malloc(super->data_bitmap_len * UFS_BLOCK_SIZE);
  readDataBitmap(super, data_bitmap);

  vector<unsigned int> valid_blocks;
  int bytes_left = my_inode->size;
  
  // Copy the direct ptrs
  unsigned int ptrs[DIRECT_PTRS];
  for (int i = 0; i < DIRECT_PTRS; ++i) {
      ptrs[i] = my_inode->direct[i];
  }  
  // read the bitmaps 
  for(unsigned int ptr: ptrs){
    if(bytes_left <= 0){
      break;
    }

    valid_blocks.push_back(ptr);
    bytes_left -= UFS_BLOCK_SIZE;
    
    // // Extract the bit - reorient bit shift amnt
    // int byte_shift_amnt = block / 8; 
    // int bit_shift_amnt = 7 - (block % 8);
    // char curr_byte = data_bitmap[byte_shift_amnt];
    // int extracted_bit = (curr_byte >> bit_shift_amnt) & 1;

    // if(extracted_bit == 1){
    //   valid_blocks.push_back(ptr);
    //   bytes_left -= UFS_BLOCK_SIZE;
    // }
  }

  // Free memory 
  free(super);
  free(data_bitmap);
  
  return valid_blocks;
}

