
#ifndef STUDENT_LZ78_DECOMPRESSOR_H
#define STUDENT_LZ78_DECOMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define EMPTY_STACK -2
#define FULL_STACK -1
#define PUSH_SUCCESSFUL 0

struct elem{
	
	char c;  
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
	char* stk;
	int top;
	int size;
};


extern struct encoding_parameters params;

void decompressor_init(struct decompressor_data* decompressor, int dictionary_size);
void decompress_LZ78(const char *input_filename, const char *output_file_name, int dictionary_size);
void decompress_LZW(const char * input_filename, const char * output_file_name);
void stack_init(struct stack* s, int size);

int stack_push(struct stack* s, char const c);

char stack_pop(struct stack* s);


#endif //STUDENT_LZ78_DECOMPRESSOR_H
