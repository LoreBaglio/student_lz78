#include <math.h>       /* log */
#include<unistd.h>
#include<stdlib.h>
#include<stdint.h>
#include<errno.h>
#include<stdio.h>
#include<strings.h>
#ifndef STUDENT_LZ78_SYMBOL_IO_H
#define STUDENT_LZ78_SYMBOL_IO_H
// Classe che potremmo fondere in file_io

#define EOF 257

int bits_per_code; // Set by compress and decompress

struct bitio;
int read_code(struct bitio*, uint64_t* result);
int write_code(struct bitio*, uint64_t data);
struct bitio* bitio_open(const char* filename, u_int mode);
int bitio_close(struct bitio*);
int compute_bit_to_represent(int);
int end_compressed_file();

#endif //STUDENT_LZ78_SYMBOL_IO_H
