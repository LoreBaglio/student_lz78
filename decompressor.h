
#ifndef STUDENT_LZ78_DECOMPRESSOR_H
#define STUDENT_LZ78_DECOMPRESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct elem{
	
	char c;  
	int parent; 
	//child id è la sua posizione nell'array
};

struct decompressor_data{

	struct elem* dictionary;
	int node_count;
};

void decompress(const char * input_filename, const char * output_file_name, int dictionary_size);

#endif //STUDENT_LZ78_DECOMPRESSOR_H
