#ifndef STUDENT_LZ78_COMPRESSOR_H
#define STUDENT_LZ78_COMPRESSOR_H

#include <stdlib.h>
#include "compressor.h"
#include "file_io.h"
#include "hash_table.h"
#include "encode.h"


struct compressor_data {
    struct hash_table * dictionary;
    node node_count;
};

extern u_int bits_per_code;
extern int size_block_bitio;

void compress(const char*,const char*,int);
void dictionary_init(struct compressor_data *, int, int);

#endif //STUDENT_LZ78_COMPRESSOR_H
