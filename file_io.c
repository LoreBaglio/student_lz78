#include "file_io.h"



void print_header(struct file_header* header){

    printf("Compression Algorithm Code: %d\n",header->compression_algorithm_code);
    printf("Dictionary size: %d\n",header->dictionary_size);
    printf("Symbol size: %d\n",header->symbol_size);
    printf("File Name: %s\n",header->filename);
    printf("File Size: %d\n",(int)header->file_size);
    printf("Last Modification: %s",ctime(&header->last_modification_time));
    printf("Checksum: %d\n",header->checksum);
    //printf("Compressed: %d\n", header->compressed);
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
    //header->compressed = 0;     //Compressed will be set at the end of compression
}

crc
crc32b(uint8_t const message[], int nBytes)
{
    crc  remainder = 0;
	int byte;
	uint8_t bit;

    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
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

	//read_data((void*)&(head->compressed), 1, sizeof(uint8_t), fp);
}


void step_crc(crc* remainder, char c) {

	uint8_t bit;
    /*
         * Bring the next byte into the remainder.
         */
    *remainder ^= (c << (WIDTH - 8));

    /*
     * Perform modulo-2 division, a bit at a time.
     */
    for (bit = 8; bit > 0; --bit)
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

int insert_header(const char *filename, int dictionary_size, FILE *fp, struct file_header* head) 
{
    
    int crc_offset = 0;

    get_header(filename, head, dictionary_size);

    write_data((void*)&(head->compression_algorithm_code), 1, sizeof(int8_t), fp);
 
    write_data((void*)&(head->dictionary_size), 1, sizeof(int32_t), fp);
 
    write_data((void*)&(head->symbol_size), 1, sizeof(int32_t), fp);

    write_data((void*)&(head->filename_len), 1, sizeof(int32_t), fp);

    write_data((void*)(head->filename), 1, head->filename_len, fp);
  
    write_data((void*)&(head->file_size), 1, sizeof(off_t), fp);

    write_data((void*)&(head->last_modification_time), 1, sizeof(time_t), fp);

    crc_offset = (int)ftell(fp);

    // CRC
    write_data((void*)&(head->checksum), 1, sizeof(crc), fp);

    //write_data((void*)&(head->compressed), 1, sizeof(uint8_t), fp);

    return crc_offset;

}

uint8_t check_size(FILE* compressed_file, off_t original_size, int header_size)
{
	int new_size;

   	new_size = (int)ftell(compressed_file);
	new_size -= header_size;

	if (original_size >= new_size){
		printf("compression finished successfully, original size was: %d new size is: %d\n", (int)original_size, new_size);
		return 1;
	}
	else{
		printf("compression finished unsuccessfully, original size was: %d new size is: %d\n", (int)original_size, new_size);
	    return 0;
	}
}

/*int check_header(struct file_header* head)
{
	time_t timestamp;

	if(head->compression_algorithm_code != LZ_78_CODE){
		printf("error: compression algorithm is not LZ78\n");
		return -1;
	}
	if(head->dictionary_size < MIN_DICTIONARY_SIZE || head->dictionary_size > MAX_DICTIONARY_SIZE){
		printf("error: data corruption\n");
		return -1;
	}
	if(head->symbol_size != SYMBOL_SIZE){
		printf("error: symbol format not valid\n");
		return -1;
	}
        //controllo che l'ultima modifica sia precedente all'ora corrente
	timestamp = time(NULL);
	if(difftime(timestamp, head->last_modification_time) < 0){
		printf("error: data corruption\n");
		return -1;
	}
	if(head->compressed == 0){
		return 0;
	}
	return 1;
}
*/
void check_decompression(FILE* fp, off_t original_size, crc original_crc, crc computed_crc)
{
	off_t size = ftell(fp);

	if(size != original_size){
		printf("error during decompression\nOriginal file was %d, decompressed one is %d", (int)original_size, (int)size);
	}
	if(original_crc != computed_crc){
		printf("error during decompression\nOriginal crc was %d, computed crc is %d\n", original_crc, computed_crc);
	}
}

/*void insert_header(const char* filename, int dictionary_size)
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
}*/




