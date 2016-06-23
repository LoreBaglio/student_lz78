#include "file_io.h"



void print_header(struct file_header* header){

    printf("Compression Algorithm Code: %d\n",header->compression_algorithm_code);
    printf("Dictionary size: %d\n",header->dictionary_size);
    printf("Symbol size: %d\n",header->symbol_size);
    printf("File Name: %s\n",header->filename);
    printf("File Size: %d\n",(int)header->file_size);
    printf("Last Modification: %s",ctime(&header->last_modification_time));
    printf("Checksum: %d\n",header->checksum);

}

void get_header(const char* filename, struct file_header* header,int dictionary_size) {

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
    header->filename_len = strlen(filename) + 1;
    header->file_size = file_stat.st_size;
    header->last_modification_time = file_stat.st_atim.tv_sec;
    header->checksum = 0;       //TODO Checksum

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

//funzione che controlla che il file di input sia diverso da quello di output per non sovrascrivere
void compare_filenames(const char* input_name, const char* output_name)
{
	int ret = strcmp(input_name, output_name);

	if(ret == 0){
		printf("error in specifing parameters\n");
		exit(0);
	}
}

//funzione che apre un file
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
		case APPEND:
			fp = fopen(filename, "a");
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
	if (fread(dest, n, size, fp) < size){
		exit(1);
	}
}

void write_data(void* sr, int n, int size, FILE* fp)
{
	if(fwrite(sr, n, size, fp) < size){
		exit(1);
	}
}

void insert_header(const char* filename, int dictionary_size)
{
	FILE* fp;
	struct file_header* head;
	char* text;
	int size = 0;
	
	head = (struct file_header*)malloc(sizeof(struct file_header));
	get_header(filename, head, dictionary_size);
	
	size = head->file_size;
	text = (char*)malloc(size + 1);

	fp = open_file(filename, READ);
	read_data(text, 1, head->file_size, fp);
	text[size] = '\0';
	fclose(fp);
		
	fp = open_file(filename, WRITE);

	size = sizeof(int8_t);
	write_data((void*)&(head->compression_algorithm_code), 1, size, fp);

	fclose(fp);

	fp = open_file(filename, APPEND);

	size = sizeof(int32_t);
	write_data((void*)&(head->dictionary_size), 1, size, fp);

	size = sizeof(int32_t);
	write_data((void*)&(head->symbol_size), 1, size, fp);
	
	size = sizeof(int32_t);
	write_data((void*)&(head->filename_len), 1, size, fp);
	
	size = head->filename_len;
	write_data((void*)(head->filename), 1, size, fp);

	size = sizeof(off_t);
	write_data((void*)&(head->file_size), 1, size, fp);

	size = sizeof(time_t);
	write_data((void*)&(head->last_modification_time), 1, size, fp);
	
	size = sizeof(int32_t);
	write_data((void*)&(head->checksum), 1, size, fp);

	size = strlen(text);
	write_data((void*)(text), 1, size, fp);

	fclose(fp);
}

void read_header(const char* filename)
{
	FILE* fp;
	struct file_header* head;
	int size;

	head = (struct file_header*)malloc(sizeof(struct file_header));
	
	fp = open_file(filename, READ);
	
	size = sizeof(int8_t);
	read_data((void*)&(head->compression_algorithm_code), 1, size, fp);

	size = sizeof(int32_t);
	read_data((void*)&(head->dictionary_size), 1, size, fp);

	size = sizeof(int32_t);
	read_data((void*)&(head->symbol_size), 1, size, fp);

	size = sizeof(int32_t);
	read_data((void*)&(head->filename_len), 1, size, fp);

	size = head->filename_len;
	head->filename = (char*)malloc(size);
	read_data((void*)(head->filename), 1, size, fp);

	size = sizeof(off_t);
	read_data((void*)&(head->file_size), 1, size, fp);

	size = sizeof(time_t);
	read_data((void*)&(head->last_modification_time), 1, size, fp);
	
	size = sizeof(int32_t);
	read_data((void*)&(head->checksum), 1, size, fp);
	
	//print_header(head);
}

