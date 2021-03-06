#include "compressor.h"


/**
* This function is called by main program and compress the file given as an input
*/
void compress(const char * input_filename, const char* output_file_name, int dictionary_size) {

    	struct compressor_data *compressor = malloc(sizeof(struct compressor_data));

    	FILE *input_fp;

    	u_int bits_per_code;

	// Variables for each step of lookup
	node parent_node = ROOT,child_node;
	uint8_t found;
	unsigned char current_symbol;
	struct table_key *node_key;
	crc remainder = 0;
	struct bitio *bitio;
	int ret;
	int header_size;
	struct file_header *header = (struct file_header *) malloc(sizeof(struct file_header));
	unsigned char* file_content;
	int count = 0;

	// Set encoding number of bits 
	bits_per_code = compute_bit_to_represent(dictionary_size);

	// Prepare all characters as first children of the root of the tree
	dictionary_init(compressor, ASCII_ALPHABET, dictionary_size);

	//Init bitio
	bitio = bitio_open(output_file_name, WRITE);
	if (bitio == NULL) {
		printf("Cannot open file %s in write mode\n", output_file_name);
		if(verbose_flag)
			printf("Compression interrupted\n");
		exit(1);
	}

	// Open input file (which will be compressed)
	input_fp = open_file(input_filename, READ);

	if (input_fp == NULL) {
		printf("Cannot open file %s in read mode\n", input_filename);
		if(verbose_flag)
			printf("Compression interrupted\n");
		exit(1);
	}

	node_key = calloc(1, sizeof(struct table_key));

	get_header(input_filename, header, dictionary_size);
	header_size = write_header(bitio->f, header);

	file_content = (unsigned char*)malloc(header->file_size);

	while (!feof(input_fp)) {

		// 1. Read a char (Read operation is buffered inside)
		if (fread(&current_symbol, 1, 1, input_fp) != 0) {

			// 2. Incremental CRC, computed during the compression cycle and attached to the header at the end
			step_crc(&remainder, current_symbol);

			file_content[count++] = current_symbol;		

			// Prepare key for lookup
			node_key->code = current_symbol;
			node_key->father = parent_node;

			// 3. Dictionary lookup
			child_node = get(compressor->dictionary, node_key, &found);

			// 4. Check if char is found in dictionary
			if (!found) {

				// 4.1 Parent node code emission
				ret = write_code(bitio, bits_per_code, parent_node);

				if (ret < 0){
					printf("Error when writing to file %s\n", output_file_name);
					if(verbose_flag)
						printf("Compression interrupted\n");

					exit(1);
				}

				//4.2 Check if dictonary is full
				if (compressor->node_count >= dictionary_size - 1) {
					
					// 4.2.1 Reinitialize dictionary
					destroy(compressor->dictionary);
					dictionary_init(compressor, ASCII_ALPHABET, dictionary_size);


				} else {
					// 4.2.2 Increment node_count and put as new child id
					put(compressor->dictionary, node_key, ++compressor->node_count);
				}

				// Restart from one-char node
				// Prepare key for lookup
				node_key->father = ROOT;

				//4.3 Dictionary lookup
				parent_node = get(compressor->dictionary, node_key, &found);

			}

			else {
				// 5. Simply update the parent_node pointer 
				parent_node = child_node;
			}

		}
	}

	// FINAL OPERATIONS

	// Write last code and EOF code
	write_code(bitio,bits_per_code, parent_node);
	parent_node = EOF_CODE;
	write_code(bitio, bits_per_code, parent_node);

	header->checksum = remainder;
	
	// Perform final operations on output file and close bitio
	if(compressor_bitio_close(bitio, file_content, header, header_size, output_file_name) < 0){
		
		if(verbose_flag)
	    		printf("error: closure of the output file failed\n");
	    	exit(1);
	}

	// Free resources
	fclose(input_fp);

	free(file_content);

	destroy(compressor->dictionary);
	free(compressor);

	free(node_key);
	free(header);

}

/**
 * This function prepares the dictionary, all the children of the root, corresponding to all symbols of the alphabet (costly to look along all the file to
 * find just the subset of symbols that appears). Code are on bits_per_code bits in order to write only necessary
 * bits to represent dictionary nodes.
 */
void dictionary_init(struct compressor_data *compressor, int symbol_alphabet, int dictionary_size){

	compressor -> dictionary = create(dictionary_size);

	if (compressor->dictionary == NULL){
		printf("Cannot allocate dictionary of the specified size\n");
		if(verbose_flag){
	    		printf("Compression interrupted\n");
		}
		exit(1);
	}

	//0 is the ROOT, 257 is EOF_CODE
	// Dictionary is considered full when the next node to be created is 2^N - 1.
	compressor -> node_count = 1;       
	

	// Prepare all first children (256 Ascii Symbols)
	unsigned char child_symbol;
	struct table_key* node_key = malloc(sizeof(struct table_key));

	// ASCII on project assumption is the only considered alphabet
	if(symbol_alphabet == ASCII_ALPHABET){

		int end_loop = 0;
		node_key -> father = ROOT;

		// Create children
		for( child_symbol = 0; !end_loop; child_symbol++ ){

			if (child_symbol == 255)
				end_loop = 1;

			node_key->code = child_symbol;
			put(compressor -> dictionary, node_key, compressor->node_count++);

		}

		free(node_key);
	}

}

