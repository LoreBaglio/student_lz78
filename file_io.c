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
    header->checksum = 0;       //Checksum will be set at the end of compression

}

crc
crc32b(uint8_t const message[], int nBytes)
{
    crc  remainder = 0;


    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}   /* crcSlow() */

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
	int ret = fread(dest, n, size, fp);
	if(feof(fp)){
		return;
	}
	if (ret < size){
		printf("read(): error in reading data\n");
		exit(1);
	}
}

void write_data(void* sr, int n, int size, FILE* fp)
{
	int ret = fwrite(sr, n, size, fp);
	if(ret < size){
		printf("write(): error in writing data\n");
		exit(1);
	}
}

void insert_header(const char* filename, int dictionary_size)
{
	FILE* fp;
	struct file_header* head;
	void* text;
	int size = 0;
	
	head = (struct file_header*)malloc(sizeof(struct file_header));
	get_header(filename, head, dictionary_size);
	
	size = head->file_size;
	text = malloc(size);

	fp = open_file(filename, READ);
	read_data(text, 1, head->file_size, fp);
	fclose(fp);
		
	fp = open_file(filename, WRITE);

	size = sizeof(int8_t);
	write_data((void*)&(head->compression_algorithm_code), 1, size, fp);

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

	size =  head->file_size;
	write_data(text, 1, size, fp);

	fclose(fp);
}

void read_header(FILE* fp, struct file_header* head)
{

	int size;
	int ret;

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
}

void step_crc(crc* remainder, char c) {

    /*
         * Bring the next byte into the remainder.
         */
    *remainder ^= (c << (WIDTH - 8));

    /*
     * Perform modulo-2 division, a bit at a time.
     */
    for (uint8_t bit = 8; bit > 0; --bit)
    {
        /*
         * Try to divide the current data bit.
         */
        if (*remainder & TOPBIT)
        {
            *remainder = (*remainder << 1) ^ POLYNOMIAL;
        }
        else
        {
            *remainder = (*remainder << 1);
        }
    }

}

void insert_header_ottimizzato(const char *filename, int dictionary_size, FILE *fp) {

    struct file_header* head;

    int size = 0;

    head = (struct file_header*)malloc(sizeof(struct file_header));
    get_header(filename, head, dictionary_size);

    size = sizeof(int8_t);
    write_data((void*)&(head->compression_algorithm_code), 1, size, fp);

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

}





