#ifndef STUDENT_LZ78_SYMBOL_IO_H
#define STUDENT_LZ78_SYMBOL_IO_H
// Classe che potremmo fondere in file_io

struct encoding_parameters {
    int bits_per_code; // Viene settata dal compressor in quanto è dichiarata extern in compressor.h
    int eof_code;
} params;

int read_code();
int write_code();
int compute_bit_to_represent(int);
int end_compressed_file();

#endif //STUDENT_LZ78_SYMBOL_IO_H