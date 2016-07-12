#define LZ_78_CODE 78
#define SYMBOL_SIZE 8
#define READ 0
#define WRITE 1
#ifndef STUDENT_LZ78_FILE_IO_H
#define STUDENT_LZ78_FILE_IO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>

typedef uint32_t crc;  /* the width of the CRC calculation and result */

#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */
#define MIN_DICTIONARY_SIZE 256
#define MAX_DICTIONARY_SIZE 65536

extern int verbose_flag;

struct file_header {

    int8_t compression_algorithm_code;
    int32_t dictionary_size;
    int32_t symbol_size;    // The symbol size in bits (char ASCII = 8 bit, Fax = 1 bit etc. )
    int32_t filename_len;
    const char* filename;
    off_t file_size;
    time_t last_modification_time;
    crc checksum;
    uint8_t compressed;
};


void print_header(struct file_header*);
FILE* open_file(const char* filename, u_int mode);
void compare_filenames(const char* input_name, const char* output_name);
void read_data(void* dest, int n, int size, FILE* fp);
void write_data(void* sr, int n, int size, FILE* fp);
crc crc32b(uint8_t const message[], int nBytes);

/**
write_header() function writes the header of the file. It returns the length of the header.
*/
int write_header(FILE* fp, struct file_header*);

/**
read_header() function reads the header of the file. 
*/
void read_header(FILE* fp, struct file_header*);

/**
get_header() function computes the header of the file.
*/
void get_header(const char *filename, struct file_header*, int dictionary_size);

/**
step_crc() function computes the crc of the file.
*/
void step_crc(crc* , char);

/**
check_size() function is called by the compressor. It returns 1 if the compression ended successfully (the size of the compressed file is less than the size of the original file), otherwise it returns 0.
*/
uint8_t check_size(FILE* compressed_file, off_t original_size, int header_size);

/** 
check_header() function is called by the decompressor. It returns -1 if there are errors, 0 if the file is not compressed, 1 if the file is compressed and has the expected format.
*/
int check_header(struct file_header*);

/**
check_decompression() function is called by the decompressor. It checks if the size of the decompressed file is equal to the size of the original file and it checks if the crc of the file computed by the decompressor is equal to the crc of the file computed by the compressor. 
*/
void check_decompression(FILE* fp, off_t original_size, crc original_crc, crc computed_crc);

#endif //STUDENT_LZ78_FILE_IO_H
