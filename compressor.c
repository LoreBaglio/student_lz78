
struct compressor_data {
    hash_table * dictionary;
    int node_count;
    int full_dictionary;
};

void compress(const char * input_filename, const char * output_file_name, int dictionary_size) {

    struct compressor_data * compressor = malloc(sizeof(struct compressor_data));

    FILE* file_pointer;
    FILE* out_fp;
    struct file_header* header = malloc(sizeof(struct file_header));

    // Variables for each step of lookup
    uint16_t parent_node = 0;       //FIXME Assicurarsi che il dizionario sia massimo 2^16 entry (64KB)
    //FIXME parent node su 8 bit se il dizionario è a 256 entry -> usare bitio?? per scrivere parent node a 7 bit se 128 entry, 9 bit se 512 etc??
    char current_symbol;
    struct table_key * node_key;
    int child_node = 0;
    struct table_key * waiting_new_key;
    int update_dictionary;  // Used when it's necessary to create a new node on the dictionary

    compressor -> dictionary = create(dictionary_size);
    compressor -> node_count = 1;       //0 is the ROOT, 2^(BIT_PER_CODE) - 1 is EOF
    // Dictionary is considered full when the next node to be created is 2^N - 1.
    // That code is reserved for END_OF_FILE code
    compressor -> full_dictionary = 0;


    // Prepare all characters as first children of the root of the tree
    init_tree_with_first_children(compressor, ASCII_ALPHABET, dictionary_size);

    file_pointer = open_file(input_filename,READ);
    out_fp = open_file(output_file_name,WRITE);
    node_key = malloc(sizeof(table_key));
    waiting_new_key = malloc(sizeof(table_key));    // This is for new node to be created

    // ==========================
    // First step
    // ==========================

    // Read first char (Read operation is buffered inside)
    fread(&current_symbol, 1, 1, file_pointer);

    //Prepare key for lookup
    node_key->code = current_symbol;
    node_key->father = parent_node;

    // Dictionary lookup (here is populated only with first 256 characters) so it will fail at first time
    child_node = get(compressor->dictionary, node_key);
    write_data(&parent_node,1,sizeof(parent_node),out_fp);
    waiting_new_key->father = parent_node;
    update_dictionary = 1;

        while(!feof(file_pointer)) {

            // Read a char (Read operation is buffered inside)
            fread(&current_symbol, 1, 1, file_pointer);

            // Check full dictionary
            if( !full_dictionary && update_dictionary){

                waiting_new_key -> code = current_symbol;
                // Increment node_count and put as new child id
                put(compressor -> dictionary, waiting_new_key, ++compressor->node_count);
                full_dictionary = !(node_count < (dictionary_size - 1));
                update_dictionary = 0;

            }

            //Prepare key for lookup
            node_key->code = current_symbol;
            node_key->father = parent_node;

            // Dictionary lookup
            child_node = get(compressor->dictionary, node_key);
            if (child_node == NO_ENTRY_FOUND) {

                //Parent node code emission
                write_data(&parent_node,1,sizeof(parent_node),out_fp);

                if(!full_dictionary){
                    waiting_new_key->father = parent_node;
                    update_dictionary = 1;
                }
            }
            else {
                parent_node = child_node;
            }

        }

        end_compressed_file();

}

/**
 * This function prepares all the children of the root, corresponding to all symbols of the alphabet (costly to look along all the file to
 * find just the subset of symbols that appears)
 */
void init_tree_with_first_children(struct compressor_data* compressor, int symbol_alphabet, int dictionary_size){

    // Set encoding number of bits and eof code
    params.bits_per_code = compute_bit_to_represent(dictionary_size);
    params.eof_code = (1 << (params.bits_per_code)) - 1);       // FIXME Check this!

    // Prepare all first children (256 Ascii Symbols)
    char child_symbol;
    struct table_key * node_key;

    // ASCII on project assumption is the only considered alphabet
    if(symbol_alphabet == ASCII_ALPHABET){

        node_key -> code = 0;

        // Create children
        for( child_symbol=0; child_symbol<256; child_symbol++ ){

            node_key -> code = child_symbol;
            put(compressor -> dictionary, node_key, ++ compressor -> node_count);

        }
    }

}


