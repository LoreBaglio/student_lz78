#include "compressor.h"
#include "file_io.h"

struct compressor_data {
    struct hash_table * dictionary;
    int node_count;
    int full_dictionary;
};

void compress(const char * input_filename, const char * output_file_name, int dictionary_size) {

    struct compressor_data * compressor = malloc(sizeof(struct compressor_data));

    FILE* in_fp;
    FILE* out_fp;
    struct file_header* header = malloc(sizeof(struct file_header));

    // Variables for each step of lookup
    uint64_t parent_node = ROOT;
    char current_symbol;
    struct table_key * node_key;
    int child_node = 0;
    crc remainder = 0;
    struct *bitio bitio;
    int crc_header_offset = 0;

    compressor -> dictionary = create(dictionary_size);
    compressor -> node_count = 1;       //0 is the ROOT, 2^(BIT_PER_CODE) - 1 is EOF
    // Dictionary is considered full when the next node to be created is 2^N - 1.
    // That code is reserved for END_OF_FILE code
    compressor -> full_dictionary = 0;

    // Prepare all characters as first children of the root of the tree
    init_tree_with_first_children(compressor, ASCII_ALPHABET, dictionary_size);


    in_fp = open_file(input_filename, READ);
    out_fp = open_file(output_file_name,WRITE);
    node_key = malloc(sizeof(struct table_key));

    crc_header_offset = insert_header_ottimizzato(input_filename, dictionary_size, out_fp);

    //Init bitio
    bitio = bitio_open(out_fp,WRITE);

    parent_node = ROOT;

    while(!feof(in_fp)) {

        // Read a char (Read operation is buffered inside)
        fread(&current_symbol, 1, 1, in_fp);
        // Incremental CRC, computed during the compression cycle and attached to the header at the end
        step_crc(&remainder, current_symbol);

        //Prepare key for lookup
        node_key->code = current_symbol;
        node_key->father = parent_node;

        // Dictionary lookup
        child_node = get(compressor->dictionary, node_key);
        if (child_node == NO_ENTRY_FOUND) {

            //Parent node code emission
            write_code(bitio,parent_node);

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
    fclose(out_fp);
    fclose(in_fp);

    //Attach CRC
    fseek(out_fp, crc_header_offset, SEEK_SET);
    fwrite(&remainder,sizeof(crc),1,out_fp);
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
        for( child_symbol = ROOT + 1; child_symbol < EOF; child_symbol++ ){

            node_key->code = child_symbol;
            put(compressor -> dictionary, node_key, ++compressor->node_count);

        }

        ++compressor->node_count;   // Hopping the EOF node id
    }

}


