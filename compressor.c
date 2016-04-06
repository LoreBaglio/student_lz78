#include <stdlib.h>
#include "compressor.h"
#include "file_io.h"


void compress(const char * input_filename, const char * output_file_name, int dictionary_size) {

    FILE* file_pointer;
    struct file_header* header = malloc(sizeof(struct file_header));


    file_pointer = get_file(input_filename,header,dictionary_size);


}

