#include "encode.h"
#include "decompressor.h"

/*
 * Allocation of structures and set of first children of ROOT node (ASCII Symbols)
 */
void decompressor_init(struct decompressor_data *decompressor, int dictionary_size) {

    unsigned char c = 0;
    uint16_t i;
    uint8_t end_loop = 0;

    // Start to count new nodes from EOF (excluded) (0 root, 1-256 first children, 257 EOF)
    decompressor -> node_count = EOF_CODE;

	decompressor->dictionary = (struct elem *) calloc(dictionary_size, sizeof(struct elem));

    //Init Root Node
    decompressor->dictionary[0].c = '\0';
    decompressor->dictionary[0].parent = EOF_CODE;

    //Init array with Single Character nodes
    for (i = 1; !end_loop; i++) {

        decompressor->dictionary[i].c = c;
        decompressor->dictionary[i].parent = ROOT;

		if (c == 255)
			end_loop = 1;
		else
			c++;
    }
}

// ===========================================================
// STACK (Used to print in reverse order branch of characters)
// ===========================================================
/*
 * Init stack
 */
void stack_init(struct stack* s, int size) {

	s->stk = calloc(size, sizeof(unsigned char));
	if (s->stk == NULL) {
		printf("Cannot allocate memory for the stack\n");
		exit(1);
	}
	s->top = -1;
	s->size = size;

}

/*
 * Push symbol (from Leaf to the Root)
 */
int stack_push(struct stack* s, unsigned char const c) {

    if (s->top == s->size - 1){
	printf("Stack is full\n");
	exit(1);
    }

    s->stk[++s->top] = c;

    return PUSH_SUCCESSFUL;
}

/*
 * Pop symbol (from Root to the Leaf)
 */
unsigned char stack_pop(struct stack* s) {

    if (s->top == -1) {
	printf("Stack is empty\n");
	exit(1);
    }

    return s->stk[s->top--];
}

// ===========================================================

// Decompressor main function using LZW Algorithm
void decompress(const char *input_filename, const char *output_file_name) {

	// I/O Structures
    FILE* output_file;
	struct bitio* bitio;
	struct file_header* header = calloc(1, sizeof(struct file_header));

	// Flags
	int is_compressed;
	int stop = 0;
	int ret;

	// Algorithm Variables
	int32_t dictionary_size;
	struct decompressor_data* decompressor;
	node current_node, previous_node = ROOT;
	node index = 0;

	// It's the first symbol of the symbol sequence corresponding to the last read code
    unsigned char extracted_parent_symbol = 0;

	// Number of bits to represent a node of the dictionary
	u_int bits_per_code;

	// CRC updated step-by-step during decompression cycle
    crc remainder = 0;

    //Init bitio (with input file)
    bitio = bitio_open(input_filename,READ);
    if (bitio == NULL){
        printf("Cannot open input file %s\n", input_filename);

		if(verbose_flag)
			printf("Decompression interrupted\n");

		exit(1);
    }

	// Open output file
    output_file = open_file(output_file_name, WRITE);

    if (output_file == NULL) {
        printf("Cannot open file %s in write mode\n", output_file_name);

		if(verbose_flag)
			printf("Decompression interrupted\n");

		exit(1);
    }

	// Get the header and check if the file is compressed or not
    read_header(bitio->f, header);
    is_compressed = check_header(header);

	// Error
    if(is_compressed == -1){

		printf("Decompression failed\n");

			if(verbose_flag)
				printf("error: input file is corrupted\n");

		exit(1);
    }

	// File is not compressed. It's the original plain file
    if(is_compressed == 0){

		// Read body of input file and check CRC
		crc checksum;
		unsigned char* text = (unsigned char*)malloc(header->file_size);
		read_data(text, 1, header->file_size, bitio->f);
		write_data(text, 1, header->file_size, output_file);

		// Check CRC
		checksum = crc32b(text, header->file_size);
		check_decompression(output_file, header->file_size, header->checksum, checksum);
		if(verbose_flag)
			printf("Decompression finished\n");

		// Free resources
		free(text);
		free(header->filename);
		free(header);
		bitio_close(bitio);
		fclose(output_file);

		return;
    }

    // Get the dictionary size from the header of the compressed file
    dictionary_size = header -> dictionary_size;

    // Init stack for decompression
    struct stack* s = calloc(1, sizeof(struct stack));
    stack_init(s, dictionary_size);

	// Init decompressor structure
	decompressor = calloc(1, sizeof(struct decompressor_data));
    decompressor_init(decompressor, dictionary_size);

    // Set encoding number of bits (to avoid waste of bits)
    bits_per_code = compute_bit_to_represent(dictionary_size);

	// ================================
	// DECOMPRESSION CYCLE
	// ================================

    while(!feof(bitio->f) || (stop != 0)){

		// 1. Read the Code <NODE_CODE>
		ret = read_code(bitio, bits_per_code, &current_node);
		stop = ret;
		if(ret < 0){
			printf("Error: corrupted code\n");

			if(verbose_flag)
				printf("Decompression interrupted\n");

			exit(1);
		}

		// Check if it's the EOF Code and the node sequence is finished
		if (current_node == EOF_CODE) {

			if(verbose_flag)
				printf("Decompression finished\n");

			break;
		}

		// Index of the array is a Leaf of the Tree.
		// The Element of the array is the Parent Node (Code and Symbol)
		index = current_node;


		// 2. Check if the index is in the dictionary. If not, add the node.
		if (index > decompressor->node_count) {

			// 3. Add node to the dictionary as child of previous node
			// (only if previous node is not root (FIRST ITERATION) and if the dictionary is not full)
			if (previous_node != ROOT && decompressor->node_count < dictionary_size - 1)
				add_node(decompressor, previous_node, extracted_parent_symbol);

			// Set Leaf for string emission
			index = decompressor->node_count;

			// 4. Symbol Sequence Emission
			emit_string(output_file, decompressor->dictionary, s, index, &extracted_parent_symbol, &remainder);


		} else {

			// 3. Symbol Sequence Emission
			emit_string(output_file, decompressor->dictionary, s, index, &extracted_parent_symbol, &remainder);

			// 4. Add node to the dictionary as child of previous node, using the first symbol of the current node
			if (previous_node != ROOT){
				if (decompressor->node_count < dictionary_size - 1)
					add_node(decompressor, previous_node, extracted_parent_symbol);
				else {
					// Reset dictionary: node_count points back to the last default node
					decompressor->node_count = EOF_CODE;
				}
			}

		}

		// 5. Set the received node as previous_node.
		// On the next iteration this node will achieve a new child,
		// with extracted_parent_symbol as link symbol.
		previous_node = current_node;

    }

	// =====================
	// FINAL OPERATIONS
	// =====================

	// Check size of obtained file and checksum
    check_decompression(output_file, header->file_size, header->checksum, remainder);

	// Close files and free resources
    if (bitio_close(bitio) < 0){

        if(verbose_flag)
            printf("error: closure of the input file failed\n");

        exit(1);
    }

    fclose(output_file); 

    //bzero(decompressor, sizeof(struct decompressor_data));
	free(decompressor->dictionary);
    free(decompressor);

    bzero(s->stk, s->size * sizeof(char));
    free(s);

	free(header->filename);
    bzero(header, sizeof(struct file_header));
    free(header);

}
/*
 * Print sequence of symbols (branch of the dictionary tree) in reverse order
 */
void emit_string(FILE *out, struct elem* dictionary, struct stack* s, node index, unsigned char *parent, crc* remainder) {

	int len = 0,i;
	unsigned char extracted_c;

	// On each iteration, check if parent node is equal to EOF_CODE (0 Node)
	// If not, push on the stack!
	while (dictionary[index].parent != EOF_CODE) {
		stack_push(s, dictionary[index].c);
		index = dictionary[index].parent;
		len++;
	}

	// Extracted_parent is the last symbol pushed into the stack (ROOT DIRECT CHILD) and so
	// the first symbol extracted from it
	*parent = stack_pop(s);
	step_crc(remainder, *parent);

	write_data(parent, 1, 1, out);

	// POP CYCLE: extract symbol, compute crc step, write the symbol into the output file
	for (i = 1; i < len; i++) {
		extracted_c = stack_pop(s);
		step_crc(remainder, extracted_c);
		write_data(&extracted_c, 1, 1, out);
	}

}

/*
 * As LZW needs, a new node corresponding to the previous sequence of symbols
 * plus the first char of the current sequence, is created
 */
void add_node(struct decompressor_data *decompressor, node previous_node, unsigned char extracted_parent) {

	node new_node_count = ++decompressor->node_count;
	decompressor->dictionary[new_node_count].parent = previous_node;
	decompressor->dictionary[new_node_count].c = extracted_parent;

}









