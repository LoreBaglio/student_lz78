#include "encode.h"
#include "decompressor.h"
#include "file_io.h"
#include "compressor.h"

void decompress_LZ78(const char *input_filename, const char *output_file_name, int dictionary_size)
{
	FILE* input_file;
	FILE* output_file;
	char* output_string;
	char received_c;
	int received_parent;
	int index = 0;
	int i = 0;
	int j;
	int len = 0;
	int offset;
	struct decompressor_data * decompressor = malloc(sizeof(struct decompressor_data));
	struct file_header* head = (struct file_header*)malloc(sizeof(struct file_header));

	decompressor -> node_count = 1;
	decompressor -> dictionary = (struct elem*)malloc(dictionary_size*sizeof(struct elem));

	//inizializzo il nodo radice
	decompressor -> dictionary[0].c = '0';
	decompressor -> dictionary[0].parent = 0;

	input_file = open_file(input_filename, READ);
	output_file = open_file(output_file_name, WRITE);

	// Set encoding number of bits and eof code
	bits_per_code = compute_bit_to_represent(dictionary_size);

	read_header(input_file, head);


	while(1){

		//decompressore legge la coppia <numero nodo corrente, carattere>

		read_data((void*)&(received_parent), 1, sizeof(int), input_file);

		//controllo se non ho letto EOF
		if(feof(input_file)){
			fclose(input_file);
			fclose(output_file);
			printf("decompression finished\n");
			break;
		}

		read_data((void*)&(received_c), 1, sizeof(char), input_file);

		decompressor -> dictionary[decompressor -> node_count].parent = received_parent;
		decompressor -> dictionary[decompressor -> node_count].c = received_c;

		//codice non ottimizzato ciclo due volte
		index = decompressor -> dictionary[decompressor -> node_count].parent;
		len++;
		while(index != 0){
			i = index;
			index = decompressor -> dictionary[i].parent;
			len++;
		}

		output_string = (char*)malloc(len);
		index = decompressor -> dictionary[decompressor -> node_count].parent;
		output_string[len - 1] = decompressor -> dictionary[decompressor -> node_count].c;
		j = 1;
		while(index != 0){
			output_string[len - 1 - j] = decompressor -> dictionary[index].c;
			i = index;
			index = decompressor -> dictionary[i].parent;
			j++;
		}

		write_data((void*)output_string, 1, len, output_file);

		len = 0;
		free(output_string);
		decompressor -> node_count++;

		if(decompressor -> node_count == dictionary_size){
			//TODO
		}

	}
}

void decompress_LZW(const char *input_filename, const char *output_file_name) {


	FILE* input_file;
	FILE* output_file;
	char* output_string;
	char received_c;
	int received_parent;
	int index = 0;
	int old_code = 0;
	uint8_t c = 0;
	int j,i,k;
	int len = 0;
	int offset;
	int32_t dictionary_size;
	struct bitio* bitio;
	struct decompressor_data * decompressor = malloc(sizeof(struct decompressor_data));
	struct file_header* head = (struct file_header*) malloc(sizeof(struct file_header));

    input_file = open_file(input_filename, READ);
    output_file = open_file(output_file_name, WRITE);

    read_header(input_file, head);

    // Get the dictionary size from the header of the compressed file
    dictionary_size = head -> dictionary_size;

    // Start to count new nodes from 257 (0 root, 1-256 first children, 257 EOF)
	decompressor -> node_count = 257;
	decompressor -> dictionary = (struct elem*) malloc(dictionary_size * sizeof(struct elem));

	//Init Root node
	decompressor -> dictionary[0].c = '0';
	decompressor -> dictionary[0].parent = 0;

	//Init array //FIXME Pensare al fatto che il primo elemento
	//problemi con le macro ROOT e EOF ho messo i numeri
	for (k = 1; k < 257; k++) {
		decompressor->dictionary[k].c = c;
		decompressor->dictionary[k].parent = 0;
		c++;
	}

	// Set encoding number of bits and eof code
	params.bits_per_code = compute_bit_to_represent(dictionary_size);

	//Init bitio
	bitio = bitio_open(input_file,READ);

	while(1){


		if(read_code(...) != bits_per_code){
		    printf("Error: corrupted code");
		    exit(1);
		}
		//TODO usare la read_code sopra
		read_data((void*)&(received_parent), 1, sizeof(int), input_file);

		//controllo se non ho letto EOF
		if(feof(input_file)){
			fclose(input_file);
			fclose(output_file);
			printf("decompression finished\n");
			break;
		}

		index = received_parent;
		//TODO fare pila

		while(index != 0){
			i = index;
			index = decompressor -> dictionary[i].parent;
			len++;
		}

		output_string = (char*)malloc(len);

		decompressor -> dictionary[decompressor -> node_count].parent = received_parent;
		index = decompressor -> dictionary[received_parent].parent;
		output_string[len - 1] = decompressor -> dictionary[received_parent].c;

		j = 1;
		while(index != 0){
			output_string[len - 1 - j] = decompressor -> dictionary[index].c;
			i = index;
			index = decompressor -> dictionary[i].parent;
			j++;
		}

		write_data((void*)output_string, 1, len, output_file);

		decompressor -> dictionary[old_code].c = output_string[0];
		old_code = decompressor -> node_count;
		decompressor -> node_count++;

		len = 0;
		free(output_string);
		

		if(decompressor -> node_count == dictionary_size){//TODO
			
		}

	}

}



