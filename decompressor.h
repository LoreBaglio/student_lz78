
#ifndef STUDENT_LZ78_DECOMPRESSOR_H
#define STUDENT_LZ78_DECOMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "file_io.h"
#define PUSH_SUCCESSFUL 0

struct elem{
	
	unsigned char c;
	node parent;
};

struct decompressor_data{

	struct elem* dictionary;
	node node_count;
};

/* Structure definition for stack */
struct stack
{
	unsigned char* stk;
	int top;
	int size;
};


extern u_int bits_per_code;
extern int verbose_flag;

void decompressor_init(struct decompressor_data* decompressor, int dictionary_size);
void decompress(const char * input_filename, const char * output_file_name);
void emit_string(FILE *out, struct elem* dictionary, struct stack* s, node index, unsigned char *parent, crc* remainder);
void add_node(struct decompressor_data* decompressor, node previous_node, unsigned char extracted_parent);

// Fuctions handling the stack
void stack_init(struct stack* s, int size);
int stack_push(struct stack* s, unsigned char const c);
unsigned char stack_pop(struct stack* s);


#endif //STUDENT_LZ78_DECOMPRESSOR_H
