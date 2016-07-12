#include "file_io.h"



void print_header(struct file_header* header){

    printf("Compression Algorithm Code: %d\n",header->compression_algorithm_code);
    printf("Dictionary size: %d\n",header->dictionary_size);
    printf("Symbol size: %d\n",header->symbol_size);
    printf("File Name: %s\n",header->filename);
    printf("File Size: %d\n",(int)header->file_size);
    printf("Last Modification: %s",ctime(&header->last_modification_time));
    printf("Checksum: %u\n",header->checksum);
    printf("Compressed: %d\n", header->compressed);
}

void get_header(const char* filename, struct file_header* header,int dictionary_size) {

    struct stat file_stat;
    int ret;

    ret = stat(filename,&file_stat);
    if(ret < 0){
        printf("Error in reading metadata of the file %s\n",filename);
        exit(1);
    }

    header->compression_algorithm_code = LZ_78_CODE;
    header->dictionary_size = dictionary_size;
    header->symbol_size = SYMBOL_SIZE;
    header->filename = filename;
    header->filename_len = strlen(filename) + 1;
    header->file_size = file_stat.st_size;
    header->last_modification_time = file_stat.st_atim.tv_sec;
    header->checksum = 0;       //Checksum will be set at the end of compression
    header->compressed = 0;     //Compressed flag will be set at the end of compression
}

void compare_filenames(const char* input_name, const char* output_name)
{
	int ret = strcmp(input_name, output_name);

	if(ret == 0){
		printf("error in specifing file names: input file must be distinct from output file\n");
		exit(0);
	}
}

FILE* open_file(const char* filename, u_int mode)
{
	FILE* fp = NULL;
	if(filename == NULL || filename[0] == '\0' || mode > 2){
		printf("open_file: filename not valid\n");
		return NULL;
	}
	switch(mode){
		case READ:
			fp = fopen(filename, "r");
			break; 
		case WRITE:
			fp = fopen(filename, "w");
			break; 
	}
	if(fp == NULL){
		printf("open_file: fopen() failed\n");
		return NULL;
	}
	return fp;
}

void read_data(void* dest, int n, int size, FILE* fp)
{
	int ret = fread(dest, size, n, fp);
	if (ret < n){
		printf("read(): error in reading data\n");
		exit(1);
	}
}

void write_data(void* sr, int n, int size, FILE* fp)
{
	int ret = fwrite(sr, size, n, fp);
	if(ret < n){
		printf("write(): error in writing data\n");
		exit(1);
	}
}


void read_header(FILE* fp, struct file_header* head)
{
	read_data((void*)&(head->compression_algorithm_code), 1, sizeof(int8_t), fp);

	read_data((void*)&(head->dictionary_size), 1, sizeof(int32_t), fp);

	read_data((void*)&(head->symbol_size), 1, sizeof(int32_t), fp);

	read_data((void*)&(head->filename_len), 1, sizeof(int32_t), fp);

	head->filename = (char*)malloc(head->filename_len);

	read_data((void*)(head->filename), 1, head->filename_len, fp);

	read_data((void*)&(head->file_size), 1, sizeof(off_t), fp);

	read_data((void*)&(head->last_modification_time), 1, sizeof(time_t), fp);
	
	read_data((void*)&(head->checksum), 1, sizeof(crc), fp);
	
	read_data((void*)&(head->compressed), 1, sizeof(uint8_t), fp);
}

void step_crc(crc* remainder, char c) {

	uint8_t bit;

        /* Bring the next byte into the remainder. */
	*remainder ^= (c << (WIDTH - 8));

	/* Perform modulo-2 division, a bit at a time. */
        for (bit = 8; bit > 0; --bit){
        	/* Try to divide the current data bit. */
		if (*remainder & TOPBIT){
		    *remainder = (*remainder << 1) ^ POLYNOMIAL;
		}
		else{
		    *remainder = (*remainder << 1);
		}
        }
}

int write_header(FILE *fp, struct file_header *header) {

	write_data((void*)&(header->compression_algorithm_code), 1, sizeof(int8_t), fp);

	write_data((void*)&(header->dictionary_size), 1, sizeof(int32_t), fp);

	write_data((void*)&(header->symbol_size), 1, sizeof(int32_t), fp);

	write_data((void*)&(header->filename_len), 1, sizeof(int32_t), fp);

	write_data((void*)(header->filename), 1, header->filename_len, fp);

	write_data((void*)&(header->file_size), 1, sizeof(off_t), fp);

	write_data((void*)&(header->last_modification_time), 1, sizeof(time_t), fp);

	// CRC
	write_data((void*)&(header->checksum), 1, sizeof(crc), fp);

	// compressed flag
	write_data((void*)&(header->compressed), 1, sizeof(uint8_t), fp);

	// Header will be not greater than 2^16 - 1
	return (int) ftell(fp);
}

uint8_t check_size(FILE* compressed_file, off_t original_size, int header_size)
{
	off_t new_size;

   	new_size = ftell(compressed_file);
	new_size -= header_size;

	if (original_size >= new_size){

		if(verbose_flag){
			printf("compression finished successfully, original size was: %ld new size is: %ld\n", original_size, new_size);
		}
		return 1;
	}
	else{
		if(verbose_flag){
			printf("compression finished unsuccessfully, original size was: %ld new size is: %ld\n", original_size, new_size);
		}
	    return 0;
	}
}

int check_header(struct file_header* head)
{
	if(head->compression_algorithm_code != LZ_78_CODE){
		if(verbose_flag){
			printf("compression algorithm is not LZ78\n");
		}
		return -1;
	}
	if(head->dictionary_size < MIN_DICTIONARY_SIZE || head->dictionary_size > MAX_DICTIONARY_SIZE){
		if(verbose_flag){
			printf("dictionary size not valid\n");
		}
		return -1;
	}
	if(head->symbol_size != SYMBOL_SIZE){
		if(verbose_flag){
			printf("symbol format not valid\n");
		}
		return -1;
	}
	if(head->compressed == 0){
		return 0;
	}
	return 1;
}

void check_decompression(FILE* fp, off_t original_size, crc original_crc, crc computed_crc)
{
	off_t size = ftell(fp);

	if(size != original_size){
		printf("error during decompression\nOriginal file was %ld, decompressed one is %ld\n", original_size, size);
	}
	if(original_crc != computed_crc){
		printf("error during decompression\nOriginal crc was %u, computed crc is %u\n", original_crc, computed_crc);
	}
}













