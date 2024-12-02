#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

#include "LocalFileSystem.h"
#include "ufs.h"

#include <string.h>

#include <stdlib.h>
#include <bitset>

using namespace std;


LocalFileSystem::LocalFileSystem(Disk *disk) {
  this->disk = disk;
}

void LocalFileSystem::readSuperBlock(super_t *super) {
    disk->readBlock(UFS_ROOT_DIRECTORY_INODE_NUMBER, super);
}

super_t* LocalFileSystem::buildSuper(){
  super_t* super_ptr = (super_t*)malloc(UFS_BLOCK_SIZE);
  readSuperBlock(super_ptr);
  return super_ptr;
}

void LocalFileSystem::readInodeBitmap(super_t *super, unsigned char *inodeBitmap) {

}

void LocalFileSystem::writeInodeBitmap(super_t *super, unsigned char *inodeBitmap) {

}

void LocalFileSystem::readDataBitmap(super_t *super, unsigned char *dataBitmap) {
  // For the length of the bitmap read bytes into inodeBitmap
  char buffer[UFS_BLOCK_SIZE];
  int len = super->data_bitmap_len;
  int num_data_bytes = (super->num_data + 8 - 1) / 8;
  // string data_bitmap_str; 


  for(int i = 0; i < len; i++){
    // read the curr block of the curr bitmap
    int start = super->data_bitmap_addr;
    disk->readBlock(start + i, buffer);    

    // Add the buffer to the string
    memcpy(dataBitmap + (UFS_BLOCK_SIZE * i), buffer, num_data_bytes);
    num_data_bytes -= UFS_BLOCK_SIZE;
  }
}

void LocalFileSystem::writeDataBitmap(super_t *super, unsigned char *dataBitmap) {

}

void LocalFileSystem::readInodeRegion(super_t *super, inode_t *inodes) {
  int inodes_addr = super->inode_region_addr;
  int inodes_len = super->inode_region_len;
  // constant size exists already 
  char buffer[UFS_BLOCK_SIZE];
  string inode_blocks;
  
  for(int i = 0; i < inodes_len; i++){
      // Read the block into the buffer
      disk->readBlock(inodes_addr + i, buffer);
      inode_blocks += buffer;
  }

  // copy the blocks into the inodes data structure
  memcpy(inodes, &inode_blocks, sizeof(inode_blocks));
}

void LocalFileSystem::writeInodeRegion(super_t *super, inode_t *inodes) {

}

int LocalFileSystem::lookup(int parentInodeNumber, string name) {
  // read the parentInodeNumber into inode struct
  inode_t inode;
  stat(parentInodeNumber, &inode);


  // declare buffers 
  int size = inode.size;
  char* my_dir_entries = new char[size];
  char buffer[UFS_BLOCK_SIZE];


  // read the direct ptr of inode to get dir_ent_t
  for(unsigned int direct_ptr: inode.direct){
    // if all the entries have been read break 
    if(size == 0){
      break;
    }

    // read the block, bytesread, and offset for dir_entry arr
    disk->readBlock(direct_ptr, buffer);
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
  super_t* super = buildSuper();

  // setting the data for inodes 
  int num_inodes = super->num_inodes;
  inode_t* inodes = (inode_t*)malloc(num_inodes * sizeof(inode_t));  // Allocate with malloc
  // inode_t* inodes = new inode_t[num_inodes];

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
  // read the super block
  super_t* super = buildSuper();

  // Allocate space for the bitmap
  //super->data_bitmap_len * UFS_BLOCK_SIZE
  unsigned char* data_bitmap =  (unsigned char*)malloc((super->num_data + 8 - 1) / 8);
  readDataBitmap(super, data_bitmap);

  // Read the inode
  inode_t* my_inode = (inode_t*)malloc(sizeof(inode_t));
  stat(inodeNumber, my_inode);
  
  // Copy the direct ptrs
  unsigned int ptrs[DIRECT_PTRS];
  for (int i = 0; i < DIRECT_PTRS; ++i) {
      ptrs[i] = my_inode->direct[i];
  }  
  // declare second buffer
  char buff[UFS_BLOCK_SIZE];
  // string data;

  // Figure out which entry in inode.direct to lookup the disk block 
  int data_block_num = 0;
  int total_bytes_read = 0;

  for(unsigned int ptr: ptrs){
    if(size == 0){
      break;
    }

    int block = int(ptr / UFS_BLOCK_SIZE);
    
    // Extract the bit
    int byte_shift_amnt = data_block_num / 8; 
    int bit_shift_amnt = data_block_num % 8;
    char curr_byte = data_bitmap[byte_shift_amnt];
    int extracted_bit = (curr_byte >> bit_shift_amnt) & 1;

    if(extracted_bit == 1){
      disk->readBlock(block, buff);
      int bytes_to_copy = min(size, UFS_BLOCK_SIZE);

      memcpy((char*)buffer, buff, bytes_to_copy);

      // read the data from the buffer into the arr
      total_bytes_read += bytes_to_copy;
      size -= bytes_to_copy;
      data_block_num++;
    }
  }

  // Free memory 
  free(super);
  free(my_inode);
  free(data_bitmap);
  // Convert bytes to blocks and get the disk block 
  // Read the block data from the disk 
  // Copy the data that the user asked for
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

