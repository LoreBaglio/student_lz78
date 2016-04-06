#define LZ_78_CODE 78
#define SYMBOL_SIZE 8
#define MAGIC_CODE 7878
#ifndef STUDENT_LZ78_FILE_IO_H
#define STUDENT_LZ78_FILE_IO_H

#include <stdio.h>




struct file_header {

    int8_t compression_algorithm_code;
    int32_t dictionary_size;
    int32_t symbol_size;    // The symbol size in bits (char ASCII = 8bit, Fax = 1 bit etc. )
    const char* filename;
    off_t file_size;
    time_t last_modification_time;
    int32_t checksum;

};

FILE* get_file(const char*, struct file_header*,int);
void print_header(struct file_header*);
// http://stackoverflow.com/questions/21001659/crc32-algorithm-implementation-in-c-without-a-look-up-table-and-with-a-public-li
//uint32_t crc32b(const char *);


#endif //STUDENT_LZ78_FILE_IO_H



