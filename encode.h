#include <math.h>      /* log */
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include "file_io.h"

#ifndef STUDENT_LZ78_SYMBOL_IO_H
#define STUDENT_LZ78_SYMBOL_IO_H

#define EOF_CODE 257
#define ROOT 0
#define ASCII_ALPHABET 1     // Assumption for the project: symbols are characters

u_int bits_per_code; // Number of bits for representing a node of the dictionary

// Typedef for type of node counters
typedef uint64_t node;

struct bitio {
    FILE* f;
    uint64_t data;
    u_int wp;
    u_int rp;
    u_int mode;
};

/**
read_code() reads size bits from the file specified in the bitio structure storing them at the location given by result. On success the number of read bits is returned, on error -1 is returned.
Errors:
    EINVAL at least one of the arguments was not valid
    ENODATA fread() function failed
*/
int read_code(struct bitio*, int size, uint64_t* result);

/**
write_code() writes size bits in the file specified in the bitio structure obtaining them from data. On success 0 is returned, on error -1 is returned.
Errors:
    EINVAL at least one of the arguments was not valid
    ENOSPC fwrite() function failed
*/
int write_code(struct bitio*, int size, uint64_t data);

/**
bitio_open() allocates and initializes a bitio structure. It returns a pointer to the bitio structure. If the allocation fails, it returns NULL.
Errors:
    EINVAL at least one of the arguments was not valid
    ENOMEM calloc() function failed
*/
struct bitio* bitio_open(const char* filename, u_int mode);

/**
bitio_close() frees the bitio structure. It overwrites the data before the memory is returned to the system. On success 0 is returned, on error -1 is returned.
Errors:
    EINVAL the argument was not valid
*/
int bitio_close(struct bitio*);

/**
compressor_bitio_close() finalizes all the operations that are pending and frees the bitio structure. It overwrites the data before the memory is returned to the system. On success 0 is returned, on error -1 is returned.
Errors:
    EINVAL the argument was not valid
*/
int compressor_bitio_close(struct bitio *, unsigned char *content, struct file_header *header, int header_size, const char *output_file);

/**
compute_bit_to_represent() computes the number of bits for representing a node of the dictionary.
*/
u_int compute_bit_to_represent(int);

#endif //STUDENT_LZ78_SYMBOL_IO_H
