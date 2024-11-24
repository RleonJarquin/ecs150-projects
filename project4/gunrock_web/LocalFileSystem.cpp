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
  char buffer[4000];
  string inodes_blocks;
  
  for(int i = 0; i < inodes_len; i++){
      // Read the block into the buffer
      disk -> readBlock(inodes_addr + i, buffer);
      inodes_blocks += buffer;
  }

  // copy the blocks into the inodes data structure
  memcpy(inodes, &inodes_blocks, inodes_len * 4000);
}

void LocalFileSystem::writeInodeRegion(super_t *super, inode_t *inodes) {

}

int LocalFileSystem::lookup(int parentInodeNumber, string name) {
  // read the super block
  super_t* super;
  readSuperBlock(super);

  // setting the data for inodes 
  int num_inodes = super -> num_inodes;
  inode_t* inodes = new inode_t[num_inodes * 4000];

  // Reading the inodes block
  readInodeRegion(super, inodes);

  // find the specific inode with the associated string name
  return 0;
}

int LocalFileSystem::stat(int inodeNumber, inode_t *inode) {
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

