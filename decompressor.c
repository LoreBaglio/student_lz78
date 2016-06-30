#include "encode.h"
#include "decompressor.h"
#include "file_io.h"
#include "compressor.h"



void decompressor_init(struct decompressor_data *decompressor, int dictionary_size, uint8_t already_init) {
	unsigned char c = 0;
    uint16_t k;
	uint8_t end_loop = 0;

    // Start to count new nodes from EOF (excluded) (0 root, 1-256 first children, 257 EOF)
    decompressor -> node_count = EOF_CODE;

	if (already_init == 0)
		decompressor->dictionary = (struct elem *) calloc(dictionary_size, sizeof(struct elem));
	else
		memset(decompressor->dictionary, 0,  dictionary_size * sizeof(struct elem));

    //inizializzo il nodo radice
    decompressor->dictionary[0].c = '\0';
    decompressor->dictionary[0].parent = EOF_CODE;

    //Init array //FIXME Pensare al fatto che il primo elemento
    for (k = 1; !end_loop; k++) {

        decompressor->dictionary[k].c = c;
        decompressor->dictionary[k].parent = ROOT;

		if (c == 255)
			end_loop = 1;
		else
        	c++;
    }
}



// Function handling stack

int stack_push(struct stack* s, unsigned char const c) {

	// fixme Debug
    if (s->top == s->size - 4){
		printf("Stack is full\n");
		return -1;
	}

    s->stk[++s->top] = c;

    return PUSH_SUCCESSFUL;
}

void stack_init(struct stack* s, int size) {

    s->stk = calloc(size, sizeof(unsigned char));
    if (s->stk == NULL) {
        printf("Cannot allocate memory for the stack\n");
        exit(1);
    }
    s->top = -1;
    s->size = size;

}

unsigned char stack_pop(struct stack* s) {

    if (s->top == -1) {
		printf("Stack is empty\n");
		exit(1);
	}

    return s->stk[s->top--];
}

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
	decompressor -> dictionary = (struct elem*)calloc(dictionary_size, sizeof(struct elem));

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

	FILE* output_file;
    unsigned char extracted_parent = 0, extracted_c;
	int current_node, previous_node = ROOT;
	int index = 0;
	int i, ret;
	int len = 0;
    int new_node_count;
    int32_t dictionary_size;
    uint64_t is_compressed;
    struct bitio* bitio;
	struct decompressor_data * decompressor = calloc(1, sizeof(struct decompressor_data));
	struct file_header* header = calloc(1, sizeof(struct file_header));
	uint8_t end_update = 0;

	//int result;

	//Init bitio
	bitio = bitio_open(input_filename,READ);
	if (bitio == NULL){
		printf("Cannot open input file %s\n", input_filename);
		exit(1);
	}

   	output_file = open_file(output_file_name, WRITE);

   	read_header(bitio->f, header);

   	//read_code(bitio,1,&is_compressed);

   	/*if(is_compressed == 0){
   	    // FIXME don't decompress
   	}*/

        /*FIXME se usiamo compressed nell'header

        result = check_header(header);
	if(result == -1){
		//error
	}
	if(result == 0){
		//don't decompress
	}
        */

   	// Get the dictionary size from the header of the compressed file
    dictionary_size = header -> dictionary_size;

    //inizializzo la pila per la decompressione
    struct stack* s = calloc(1, sizeof(struct stack));
    stack_init(s, dictionary_size);

    decompressor_init(decompressor, dictionary_size, 0);
	new_node_count = decompressor->node_count;

	// Set encoding number of bits and eof code
	bits_per_code = compute_bit_to_represent(dictionary_size);


	while(!feof(bitio->f)){

//		ret = read_code(bitio, bits_per_code, &current_node);
		/*if(ret != bits_per_code){
		    printf("Error: corrupted code");        //Fixme è corretto questo check?
		    exit(1);
		}*/
		if (fread(&current_node, sizeof(int), 1 , bitio->f) != 0) {

			//controllo se non ho letto EOF     //FIXME decidere definitivamente se usare nodo EOF o semplicemente feof
			/*if (current_node == EOF_CODE) {
				printf("Decompression finished\n");
				break;
			}*/

			index = current_node;


			//Check if the index is in the dictionary
			if (index > decompressor->node_count) {

				// Aggiungo nodo al dizionario
				if (previous_node != ROOT && decompressor->node_count < dictionary_size)
					add_node(decompressor, previous_node, extracted_parent);

				index = decompressor->node_count;


				emit_string(output_file, decompressor->dictionary, s, index, &extracted_parent);


			} else {
				// Ad ogni ciclo controllo che il parent non sia EOF_CODE
				// In tal caso push sulla pila

				emit_string(output_file, decompressor->dictionary, s, index, &extracted_parent);

				// Aggiungo nodo al dizionario
				if (previous_node != ROOT && decompressor->node_count < dictionary_size)
					add_node(decompressor, previous_node, extracted_parent);

			}

			// Imposto il nodo ricevuto come prossimo nodo al quale aggiungeremo il figlio
			previous_node = current_node;

			// Dizionario pieno: svuotare tutto
			if (decompressor->node_count >= dictionary_size) {
				/*	free(decompressor->dictionary);
                decompressor_init(decompressor, dictionary_size, 0);*/
				previous_node = ROOT;
				//end_update = 1;

			}
		}
	}

    //alla fine della decompressione controllo che la dimensione del file decompresso sia uguale a quella originale
    check_decompression(output_file, header->file_size);
   
    bitio_close(bitio);
    fclose(output_file);

    bzero(decompressor, sizeof(struct decompressor_data));
    free(decompressor);

    bzero(s->stk, s->size * sizeof(char));
    free(s);

    bzero(header, sizeof(struct file_header));
    free(header);

}

void emit_string(FILE *out, struct elem* dictionary, struct stack* s, int index, unsigned char *parent) {

	int len = 0,i;
	unsigned char extracted_c;


	while (dictionary[index].parent != EOF_CODE) {
		stack_push(s, dictionary[index].c);
		index = dictionary[index].parent;
		len++;
	}

	*parent = stack_pop(s);

	write_data(parent, 1, 1, out);

	// Ciclo di estrazione (non vorrei aver esagerato con l'ottimizzazione)
	for (i = 1; i < len; i++) {
		extracted_c = stack_pop(s);
		write_data(&extracted_c, 1, 1, out);
	}

}

void add_node(struct decompressor_data *decompressor, int previous_node, unsigned char extracted_parent) {

	int new_node_count = ++decompressor->node_count;
	decompressor->dictionary[new_node_count].parent = previous_node;
	// extracted_parent "dovrebbe" puntare all'ultimo carattere estratto dalla pila
	// quindi al carattere giusto da aggiungere come ultimo figlio letto
	// (primo nella sequenza di pop)
	decompressor->dictionary[new_node_count].c = extracted_parent;

}









