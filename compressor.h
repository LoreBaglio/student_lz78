#ifndef STUDENT_LZ78_COMPRESSOR_H
#define STUDENT_LZ78_COMPRESSOR_H

#include <stdlib.h>
#include "compressor.h"
#include "file_io.h"
#include "hash_table.h"
#include "encode.h"

#define ASCII_ALPHABET 1     // Assumption for the project: symbols are characters
#define ROOT 0

struct compressor_data;
extern struct encoding_parameters params;

void compress(const char*,const char*,int);
void init_tree_with_first_children(struct compressor_data*, int, int);

#endif //STUDENT_LZ78_COMPRESSOR_H
