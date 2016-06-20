#include <stdlib.h>
#include "compressor.h"
#include "file_io.h"
#include "hash_table.h"

struct compressor_data {
    hash_table * dictionary;
    int node_count;
};

void compress(const char * input_filename, const char * output_file_name, int dictionary_size) {

    struct compressor_data * compressor = malloc(sizeof(struct compressor_data));

    FILE* file_pointer;
    struct file_header* header = malloc(sizeof(struct file_header));

    // Variables for each step of lookup
    int parent_node = 0;
    char current_symbol;
    struct table_key * node_key;
    int child_node = 0;
    struct table_key * waiting_new_key;
    int first_step = 1;
    int first_global_step = 1;  // FIXME Orribile variabile non più usata. Potrebbe essere ottimizzata sta cosa

    compressor -> dictionary = create(dictionary_size);
    compressor -> node_count = 1;

    // Prepare all characters as first children of the root of the tree
    init_tree_with_first_children(compressor,ASCII_ALPHABET);

    file_pointer = get_file(input_filename,header,dictionary_size);
    node_key = malloc(sizeof(table_key));
    waiting_new_key = malloc(sizeof(table_key));    // This is for new node to be created

        while(1) {

            // Read a char (Read operation is buffered inside)
            fread(&current_symbol, 1, 1, file_pointer);

            //TODO CHECK EOF

            if(first_step && !first_global_step){
                waiting_new_key -> code = current_symbol;
                // TODO Controllare il caso in cui il dizionario è saturo e non va più aggiornato! -> condizione node_count == dictionary_size
                // Increment node_count and put as new child id
                put(compressor -> dictionary, waiting_new_key, ++compressor->node_count);
                first_step = 0;
            }

            // FIXME Orribile condizione controllata ogni volta -> OTTIMIZZAR
            if(first_global_step){
                first_global_step = 0;
            }

            //Prepare key for lookup
            node_key->code = current_symbol;
            node_key->father = parent_node;


            // Dictionary lookup
            child_node = get(compressor->dictionary, node_key);
            if (child_node == NO_ENTRY_FOUND) {
                // TODO <emettere codice parent_node>
                waiting_new_key->father = parent_node;
                first_step = 1;
            }
            else {
                parent_node = child_node;
            }

    }


}

/**
 * This function prepares all the children of the root, corresponding to all symbols of the alphabet (costly to look along all the file to
 * find just the subset of symbols that appears)
 */
void init_tree_with_first_children(struct compressor_data* compressor, int symbol_alphabet){

    // FIXME Cosa ci si mette come primi figli? Tutti e 256 i codici o solo le lettere? SOTTO HO MESSO TUTTO
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


