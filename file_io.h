#define LZ_78_CODE 78
#define SYMBOL_SIZE 8
#define MAGIC_CODE 7878
#define READ 0
#define WRITE 1
#define APPEND 2
#ifndef STUDENT_LZ78_FILE_IO_H
#define STUDENT_LZ78_FILE_IO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>


struct file_header {

    int8_t compression_algorithm_code;
    int32_t dictionary_size;
    int32_t symbol_size;    // The symbol size in bits (char ASCII = 8bit, Fax = 1 bit etc. )
    int32_t filename_len;
    const char* filename;
    off_t file_size;
    time_t last_modification_time;
    int32_t checksum;

};

void get_header(const char*, struct file_header*,int);
void print_header(struct file_header*);
FILE* open_file(const char* filename, u_int mode);
void compare_filenames(const char* input_name, const char* output_name);
void read_data(void* dest, int n, int size, FILE* fp);
void write_data(void* sr, int n, int size, FILE* fp);
void insert_header(const char* filename, int dictionary_size);
int read_header(struct file_header*, const char* filename);
// http://stackoverflow.com/questions/21001659/crc32-algorithm-implementation-in-c-without-a-look-up-table-and-with-a-public-li
//uint32_t crc32b(const char *);


#endif //STUDENT_LZ78_FILE_IO_H
