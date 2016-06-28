#include "compressor.h"
#include "file_io.h"


void compress(const char * input_filename, const char* output_file_name, int dictionary_size) {

    struct compressor_data * compressor = malloc(sizeof(struct compressor_data));

    FILE* input_fp;

    // Variables for each step of lookup
    int parent_node = ROOT;
    char current_symbol;
    struct table_key * node_key;
    int child_node = 0;
    crc remainder = 0;
    struct bitio* bitio;
    int crc_header_offset = 0;
    int ret;

    compressor -> dictionary = create(dictionary_size);

    if (compressor->dictionary == NULL){
        printf("Cannot allocate dictionary of the specified size\n");
        exit(1);
    }

    compressor -> node_count = 1;       //0 is the ROOT, 2^(BIT_PER_CODE) - 1 is EOF
    // Dictionary is considered full when the next node to be created is 2^N - 1.
    // That code is reserved for END_OF_FILE code
    compressor -> full_dictionary = 0;

    // Prepare all characters as first children of the root of the tree
    init_tree_with_first_children(compressor, ASCII_ALPHABET, dictionary_size);

    //Init bitio
    bitio = bitio_open(output_file_name,WRITE);
    if (bitio == NULL){
        printf("Cannot open file %s in write mode\n", output_file_name);
        exit(1);
    }

    input_fp = open_file(input_filename, READ);

    if (input_fp == NULL){
        printf("Cannot open file %s in read mode\n", input_filename);
        exit(1);
    }

    node_key = calloc(1, sizeof(struct table_key));

    crc_header_offset = insert_header(input_filename, dictionary_size, bitio->f);

    while(!feof(input_fp)) {

        // Read a char (Read operation is buffered inside)
        fread(&current_symbol, 1, 1, input_fp);
        // Incremental CRC, computed during the compression cycle and attached to the header at the end
        step_crc(&remainder, current_symbol);

        //Prepare key for lookup
        node_key->code = current_symbol;
        node_key->father = parent_node;

        // Dictionary lookup
        child_node = get(compressor->dictionary, node_key);
        if (child_node == NO_ENTRY_FOUND) {

            //Parent node code emission
            ret = write_code(bitio,parent_node);

            if (ret < 0){
                printf("Error when writing to file %s\n", output_file_name);
                exit(1);
            }

            if(!compressor->full_dictionary){
                // Increment node_count and put as new child id
                put(compressor -> dictionary, node_key, ++compressor->node_count);
                // Restart from one-char node
                node_key->father = ROOT;
                parent_node = get(compressor->dictionary, node_key);
            }
            else {
                destroy(compressor->dictionary);
                compressor->dictionary = create(dictionary_size);
                compressor->node_count = 1;
                compressor->full_dictionary = 0;
                init_tree_with_first_children(compressor, ASCII_ALPHABET, dictionary_size);
            }
        }
        else {
            parent_node = child_node;
        }

    }

    //end_compressed_file();
    bitio_close(bitio);
    fclose(input_fp);

    destroy(compressor->dictionary);
    free(compressor);

    //Attach CRC
    fseek(bitio->f, crc_header_offset, SEEK_SET);
    fwrite(&remainder,sizeof(crc),1,bitio->f);
}

/**
 * This function prepares all the children of the root, corresponding to all symbols of the alphabet (costly to look along all the file to
 * find just the subset of symbols that appears)
 */
void init_tree_with_first_children(struct compressor_data* compressor, int symbol_alphabet, int dictionary_size){

    // Set encoding number of bits and eof code
    bits_per_code = compute_bit_to_represent(dictionary_size);

    // Prepare all first children (256 Ascii Symbols)
    char child_symbol;
    struct table_key * node_key = calloc(1, sizeof(struct table_key));

    // ASCII on project assumption is the only considered alphabet
    if(symbol_alphabet == ASCII_ALPHABET){

        node_key -> father = ROOT;

        // Create children
        for( child_symbol = ROOT + 1; child_symbol < EOF_CODE; child_symbol++ ){

            node_key->code = child_symbol;
            put(compressor -> dictionary, node_key, ++compressor->node_count);

        }

        ++compressor->node_count;   // Hopping the EOF node id
    }

}


