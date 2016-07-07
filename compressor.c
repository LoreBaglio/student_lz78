#include "compressor.h"
#include "file_io.h"


void compress(const char * input_filename, const char* output_file_name, int dictionary_size) {

    struct compressor_data *compressor = malloc(sizeof(struct compressor_data));

    FILE *input_fp;

    // Variables for each step of lookup
    node parent_node = ROOT,child_node;
    uint8_t found;
    unsigned char current_symbol;
    struct table_key *node_key;
    crc remainder = 0;
    struct bitio *bitio;
    int crc_header_offset = 0;
    int ret;
    uint8_t is_compressed;
    uint64_t tmp;
    int header_size;
    struct file_header *head = (struct file_header *) malloc(sizeof(struct file_header));
    uint8_t end_update = 0;
    unsigned char* file_content;
    int count = 0;

    // Prepare all characters as first children of the root of the tree
    dictionary_init(compressor, ASCII_ALPHABET, dictionary_size);

    //Init bitio
    bitio = bitio_open(output_file_name, WRITE);
    if (bitio == NULL) {
        printf("Cannot open file %s in write mode\n", output_file_name);
        exit(1);
    }

    input_fp = open_file(input_filename, READ);

    if (input_fp == NULL) {
        printf("Cannot open file %s in read mode\n", input_filename);
        exit(1);
    }

    node_key = calloc(1, sizeof(struct table_key));

    crc_header_offset = insert_header(input_filename, dictionary_size, bitio->f, head);
    file_content = (unsigned char*)malloc(head->file_size);
    // Write is_compressed and in case of compressed file larger than original,
    // reset the flag at the end
    //write_code(bitio,1,1);

    while (!feof(input_fp)) {

        // Read a char (Read operation is buffered inside)
        if (fread(&current_symbol, 1, 1, input_fp) != 0) {

            // Incremental CRC, computed during the compression cycle and attached to the header at the end
            step_crc(&remainder, current_symbol);
            
	    // memorizzo il simbolo letto nel caso in cui non debba inviare il file compresso ma quello originale 
	    //FIXME o invece è meglio riaprire il file e rileggerlo?
            file_content[count++] = current_symbol;		

            //Prepare key for lookup
            node_key->code = current_symbol;
            node_key->father = parent_node;

            // Dictionary lookup
            child_node = get(compressor->dictionary, node_key, &found);

            if (!found) {

                //Parent node code emission
                ret = write_code(bitio, bits_per_code, parent_node);

                if (ret < 0){
                    printf("Error when writing to file %s\n", output_file_name);
                    exit(1);
                }

                if (compressor->node_count < dictionary_size) {

                    // Increment node_count and put as new child id
                    put(compressor->dictionary, node_key, ++compressor->node_count);

                    // Restart from one-char node
                    node_key->father = ROOT;
                    parent_node = get(compressor->dictionary, node_key, &found);
                }
                else {
                    /*destroy(compressor->dictionary);
                    dictionary_init(compressor, ASCII_ALPHABET, dictionary_size);

                    // Increment node_count and put as new child id
                    put(compressor -> dictionary, node_key, ++compressor->node_count);*/
                    // Restart from one-char node
                    node_key->father = ROOT;
                    parent_node = get(compressor->dictionary, node_key, &found);
                }
            }
            else {
                parent_node = child_node;
            }

        }
    }

    // Write last code and EOF code
    write_code(bitio,bits_per_code, parent_node);
    parent_node = EOF_CODE;
    write_code(bitio, bits_per_code, parent_node);

    //Attach CRC
    fseek(bitio->f, crc_header_offset, SEEK_SET);
    write_data(&remainder, 1, sizeof(crc), bitio->f);

    /*if(is_compressed == 0){

        // FIXME TEST THIS!
        // posizione su bit is compressed
        fseek(bitio->f, header_size, SEEK_SET);

        // Leggo 64 bit
        fread(&tmp, 1, size_bitio_block, bitio->f);

        // Setto il primo bit a 0
        tmp &= (1 << (size_bitio_block * 8 - 1)) - 1;

        // Riscrivo il byte
        fseek(bitio->f, header_size, SEEK_SET);
        fwrite(&tmp, 1, size_bitio_block, bitio->f);

    }*/

    //end_compressed_file();
    fseek(bitio->f, 0, SEEK_END);
    header_size = crc_header_offset + sizeof(crc) + sizeof(uint8_t);
    //FIXME la bitio_close potrebbe fare una write e cambiare la dimensione del file compresso ho pensato di controllare la dimensione all'interno della close()
    compressor_bitio_close(bitio, file_content, head->file_size, header_size);
    fclose(input_fp);

    free(file_content);

    destroy(compressor->dictionary);
    free(compressor);

    free(node_key);
    free(head);

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
        exit(1);
    }

    compressor -> node_count = 1;       //0 is the ROOT, 2^(BIT_PER_CODE) - 1 is EOF
    // Dictionary is considered full when the next node to be created is 2^N - 1.
    // That code is reserved for END_OF_FILE code

    // Set encoding number of bits and eof code
    bits_per_code = compute_bit_to_represent(dictionary_size);

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

        //print_table(compressor->dictionary);

        //++compressor->node_count;   // Hopping the EOF node id
    }

}

