#include <math.h>      /* log */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>

#ifndef STUDENT_LZ78_SYMBOL_IO_H
#define STUDENT_LZ78_SYMBOL_IO_H
// Classe che potremmo fondere in file_io

#define EOF_CODE 257
#define ROOT 0
#define ASCII_ALPHABET 1     // Assumption for the project: symbols are characters

u_int bits_per_code; // Viene settata dal compressor in quanto è dichiarata extern in compressor.h
int size_bitio_block;


// Typedef for type of node counters
typedef uint64_t node;

struct bitio{
    FILE* f;
    uint64_t data;
    u_int wp;
    u_int rp;
    u_int mode;
};

int read_code(struct bitio*,int size, uint64_t* result);
int write_code(struct bitio*, int size, uint64_t data);
struct bitio* bitio_open(const char* filename, u_int mode);
int bitio_close(struct bitio*);
u_int compute_bit_to_represent(int);
int end_compressed_file();

#endif //STUDENT_LZ78_SYMBOL_IO_H
