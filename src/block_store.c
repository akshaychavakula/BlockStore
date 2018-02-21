#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need
#include "bitmap.c"

#define BITMAP_SIZE_BYTES 256   // 2^8 blocks
#define BLOCK_SIZE_BYTES 256
#define BLOCK_SIZE_BITS 2048    // 2^8 bytes per block
#define BLOCK_STORE_NUM_BLOCKS 256  //2^8 blocks
#define BLOCK_STORE_AVAIL_BLOCKS 255    // Last block consumed by the FBM   
#define BLOCK_STORE_NUM_BYTES 65536 // 2^8 blocks of 2^8 bytes

struct block_store {
    uint8_t* blocks;    //array of blocks in block storage
    bitmap_t* fbm;      //FBM stored in the first few blocks of the device 
};

block_store_t *block_store_create(){
    //memory allocation for block store
    block_store_t *bs = (block_store_t*)malloc(sizeof(block_store_t));
    //check for failed memory allocation
    if(bs == NULL){
        return NULL;
    }
    else{
        //allocate blocks array in block store struct
        bs->blocks = malloc(BLOCK_STORE_NUM_BYTES);
        //check for failed memory allocation
        if(bs->blocks == NULL){
            return NULL;
        }
        else{
            //initialize bitmap
            //set bitmap points to 0
            bs->fbm = (bitmap_t*)malloc(sizeof(bitmap_t));
            bs->fbm->data = bs->blocks;
            int i =0;
            for(i = 0; i < 256; i++){
                *(bs->fbm->data+i) = 0;
            }
            bs->fbm->leftover_bits = 255;
            bs->fbm->bit_count =255;
            bs->fbm->byte_count = 31;
            return bs;
        }
    }
}

void block_store_destroy(block_store_t *const bs){
    //destroy bs
    if(bs != NULL){
        free(bs->blocks);
        free(bs->fbm);
        free(bs);
    }
    return;
}

size_t block_store_allocate(block_store_t *const bs){
    if(bs != NULL){
        //find first empty block in bs device 
        size_t empty = bitmap_ffz(bs->fbm);
        if(empty == SIZE_MAX){
            //if no zeroes found, return size_max
            return SIZE_MAX;
        }
        //update bitmap
        bitmap_set(bs->fbm, empty);
        return empty;
    }
    else{
        return SIZE_MAX;
    }
}

bool block_store_request(block_store_t *const bs, const size_t block_id){
    //check if bs device is null and search bitmap 
    if(bs != NULL){
        if(block_id > 0 && block_id < BITMAP_SIZE_BYTES){
            if(bitmap_test(bs->fbm, block_id)){
                return false;
            }
            else{
                bitmap_set(bs->fbm, block_id);
                return true;
            }
        }
    }
    return false;   
}

void block_store_release(block_store_t *const bs, const size_t block_id){
    //reset bitmap for the released block
    if(bs != NULL){
        if(block_id < BITMAP_SIZE_BYTES){
            bitmap_reset(bs->fbm, block_id);
        }
    }
    else{
        return;
    }
}

size_t block_store_get_used_blocks(const block_store_t *const bs){
    //return total set in bitmap
    if(bs != NULL){
        return bitmap_total_set(bs->fbm);
    }
    else{
        return SIZE_MAX;
    }
}

size_t block_store_get_free_blocks(const block_store_t *const bs){
    //check for null bs device 
    if(bs != NULL){
        //return total available minus set blocks
        return BLOCK_STORE_AVAIL_BLOCKS - bitmap_total_set(bs->fbm);
    }
    else{
        return SIZE_MAX;
    }
}

size_t block_store_get_total_blocks(){
    //defined at the top 
    return BLOCK_STORE_AVAIL_BLOCKS;
}


size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer){
    //check for bad paramters
    if(bs == NULL || block_id > BITMAP_SIZE_BYTES || buffer == NULL){
        return 0;
    }
    else{
        uint8_t* temp = (bs->blocks + (block_id * BLOCK_SIZE_BYTES));
        int i = 0; //counter
        for(i = 0;i < BLOCK_SIZE_BYTES; i++){
            *((uint8_t*)buffer+i) = *(temp+i);
        }
        return i;
    }
}

size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer){
    //check for bad parameters
    if(bs == NULL || block_id > BITMAP_SIZE_BYTES || buffer == NULL){
        return 0;
    }
    else{
        //write data stored in buffer to block
        uint8_t* temp = (bs->blocks + (block_id * BLOCK_SIZE_BYTES));
        int i = 0; //counter
        for(i = 0;i < BLOCK_SIZE_BYTES;i++){
            *(temp+i) = *((uint8_t*)buffer+i);
        }
        return i;
    }
}
