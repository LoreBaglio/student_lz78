
#define ASCII_ALPHABET 1     // Assumption for the project: symbols are characters
#ifndef STUDENT_LZ78_COMPRESSOR_H
#define STUDENT_LZ78_COMPRESSOR_H

struct compressor_data;

void compress(const char*,const char*,int);
void init_tree_with_first_children(struct compressor_data*, int);

#endif //STUDENT_LZ78_COMPRESSOR_H
