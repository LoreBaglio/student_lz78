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

/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */
typedef uint32_t crc;

#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */
#define MIN_DICTIONARY_SIZE 256
#define MAX_DICTIONARY_SIZE 65536


struct file_header {

    int8_t compression_algorithm_code;
    int32_t dictionary_size;
    int32_t symbol_size;    // The symbol size in bits (char ASCII = 8bit, Fax = 1 bit etc. )
    int32_t filename_len;
    const char* filename;
    off_t file_size;
    time_t last_modification_time;
    int32_t checksum;
    //TODO decidere bit compressed
    //uint8_t compressed;
};

void get_header(const char*, struct file_header*,int);
void print_header(struct file_header*);
FILE* open_file(const char* filename, u_int mode);
void compare_filenames(const char* input_name, const char* output_name);
void read_data(void* dest, int n, int size, FILE* fp);
void write_data(void* sr, int n, int size, FILE* fp);
int insert_header(const char* filename, int dictionary_size, FILE* fp, struct file_header*);
void read_header(FILE*, struct file_header*);
crc crc32b(uint8_t const *, int);
void step_crc(crc* , char);
uint8_t check_size(FILE* compressed_file, off_t original_size, int header_size);
//int check_header(struct file_header*);
void check_decompression(FILE* fp, int original_size);

#endif //STUDENT_LZ78_FILE_IO_H
