#ifndef STUDENT_LZ78_COMPRESSOR_H
#define STUDENT_LZ78_COMPRESSOR_H

#include <stdlib.h>
#include "compressor.h"
#include "file_io.h"
#include "hash_table.h"
#include "encode.h"


struct compressor_data;
extern int bits_per_code;

void compress(const char*,const char*,int);
void init_tree_with_first_children(struct compressor_data*, int, int);

#endif //STUDENT_LZ78_COMPRESSOR_H
