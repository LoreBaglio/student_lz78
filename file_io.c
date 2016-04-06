#include <stdint.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include "file_io.h"



void print_header(struct file_header* header){

    printf("Compression Algorithm Code: %d\n",header->compression_algorithm_code);
    printf("Dictionary size: %d\n",header->dictionary_size);
    printf("Symbol size: %d\n",header->symbol_size);
    printf("File Name: %s\n",header->filename);
    printf("File Size: %d\n",header->file_size);
    printf("Last Modification: %s",ctime(&header->last_modification_time));
    printf("Checksum: %d\n",header->checksum);

}

FILE* get_file(const char* filename, struct file_header* header,int dictionary_size) {

    struct stat file_stat;
    int ret;

    ret = stat(filename,&file_stat);
    if(ret < 0){
        printf("Errore nella lettura dei metadati del file %s\n",filename);
        exit(1);
    }

    header->compression_algorithm_code = LZ_78_CODE;
    header->dictionary_size = dictionary_size;
    header->symbol_size = SYMBOL_SIZE;
    header->filename = filename;
    header->file_size = file_stat.st_size;
    header->last_modification_time = file_stat.st_atim.tv_sec;
    header->checksum = 0;       //TODO Checksum

    print_header(header);







}
/*
uint32_t crc32b(const char* message) {
    int i, j;
    unsigned int byte, crc, mask;

    i = 0;
    crc = 0xFFFFFFFF;
    while (message[i] != 0) {
        byte = message[i];            // Get next byte.
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--) {    // Do eight times.
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
        i = i + 1;
    }
    return ~crc;
}*/



