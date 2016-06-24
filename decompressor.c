
#include "decompressor.h"
#include "file_io.h"

void decompress(const char * input_filename, const char * output_file_name, int dictionary_size)
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
	struct decompressor_data * decompressor = malloc(sizeof(struct decompressor_data));

	decompressor -> node_count = 1;
	decompressor -> dictionary = (struct elem*)malloc(dictionary_size);  //FIXME dimensione dell'array è dictionary_size?

	//inizializzo il nodo radice
	decompressor -> dictionary[0].c = '0';
	decompressor -> dictionary[0].parent = 0;

	input_file = open_file(input_filename, READ);
	output_file = open_file(output_file_name, APPEND);

	while(1){

		//decompressore legge la coppia <numero nodo corrente, carattere>

		fread((void*)&(received_parent), 1, sizeof(int), input_file);
		
		//controllo se non ho letto EOF
		if(feof(input_file)){
			fclose(input_file);
			fclose(output_file);
			printf("decompression finished\n");
			break;
		}

		fread((void*)&(received_c), 1, sizeof(char), input_file);

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
		
		fwrite((void*)output_string, 1, len, output_file);

		len = 0;
		decompressor -> node_count++; 

		free(output_string);

	}	
}
//TODO controllare quando la dimensione del dizionario è troppo piccola
