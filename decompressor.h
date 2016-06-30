
#ifndef STUDENT_LZ78_DECOMPRESSOR_H
#define STUDENT_LZ78_DECOMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PUSH_SUCCESSFUL 0

struct elem{
	
	unsigned char c;
	int parent;
	//child id è la sua posizione nell'array
};

struct decompressor_data{

	struct elem* dictionary;
	int node_count;
};

/* Structure definition for stack */
struct stack
{
	unsigned char* stk;
	int top;
	int size;
};


extern u_int bits_per_code;

void decompressor_init(struct decompressor_data* decompressor, int dictionary_size, uint8_t already_init);
void decompress_LZ78(const char *input_filename, const char *output_file_name, int dictionary_size);
void decompress_LZW(const char * input_filename, const char * output_file_name);
void emit_string(FILE *out, struct elem* dictionary, struct stack* s, int index, unsigned char *parent);
void add_node(struct decompressor_data* decompressor, int previous_node, unsigned char extracted_parent);

// Fuctions handling the stack
void stack_init(struct stack* s, int size);
int stack_push(struct stack* s, unsigned char const c);

unsigned char stack_pop(struct stack* s);


#endif //STUDENT_LZ78_DECOMPRESSOR_H
