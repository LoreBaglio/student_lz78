#include "encode.h"
#include "decompressor.h"
#include "file_io.h"
#include "compressor.h"



void decompressor_init(struct decompressor_data *decompressor, int dictionary_size) {
	unsigned char c = 0;
    uint16_t k;

    // Start to count new nodes from EOF (excluded) (0 root, 1-256 first children, 257 EOF)
    decompressor -> node_count = EOF_CODE;

    decompressor->dictionary = (struct elem *) calloc(1, dictionary_size * sizeof(struct elem));

    //inizializzo il nodo radice
    decompressor->dictionary[0].c = '0';
    decompressor->dictionary[0].parent = 0;

    //Init array //FIXME Pensare al fatto che il primo elemento
    for (k = ROOT + 1; k < EOF_CODE; k++) {

        decompressor->dictionary[k].c = c;
        decompressor->dictionary[k].parent = 0;

		if (c == 255)
			break;
		else
        	c++;
    }
}



// Function handling stack

int stack_push(struct stack* s, char const c) {

    if (s->top == s->size - 1)
        return FULL_STACK;

    s->stk[++s->top] = c;

    return PUSH_SUCCESSFUL;
}

void stack_init(struct stack* s, int size) {

    s->stk = calloc(size, sizeof(char));
    if (s->stk == NULL) {
        printf("Cannot allocate memory for the stack\n");
        exit(1);
    }
    s->top = -1;
    s->size = size;

}

char stack_pop(struct stack* s) {

    if (s->top == -1)
        return EMPTY_STACK;

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
    char extracted_parent = 0, extracted_c;
	uint64_t current_node, previous_node = ROOT;
	uint64_t index = 0;
	int i, ret;
	int len = 0;
    int new_node_count;
    int32_t dictionary_size;
    uint64_t is_compressed;
    struct bitio* bitio;
	struct decompressor_data * decompressor = calloc(1, sizeof(struct decompressor_data));
	struct file_header* header = calloc(1, sizeof(struct file_header));

	//Init bitio
	bitio = bitio_open(input_filename,READ);
	if (bitio == NULL){
		printf("Cannot open input file %s\n", input_filename);
		exit(1);
	}

   	output_file = open_file(output_file_name, WRITE);

   	read_header(bitio->f, header);

   	read_code(bitio,1,&is_compressed);

   	if(is_compressed == 0){
   	    // FIXME don't decompress
   	}

   	// Get the dictionary size from the header of the compressed file
    dictionary_size = header -> dictionary_size;

    //inizializzo la pila per la decompressione
    struct stack* s = calloc(1, sizeof(struct stack));
    stack_init(s, dictionary_size);

    decompressor_init(decompressor, dictionary_size);

	// Set encoding number of bits and eof code
	bits_per_code = compute_bit_to_represent(dictionary_size);


	while(1){

		ret = read_code(bitio, bits_per_code, &current_node);
		/*if(ret != bits_per_code){
		    printf("Error: corrupted code");        //Fixme è corretto questo check?
		    exit(1);
		}*/

		//controllo se non ho letto EOF     //FIXME decidere definitivamente se usare nodo EOF o semplicemente feof
		if(feof(bitio->f)){
			printf("Decompression finished\n");
			break;
		}

		index = current_node;

        // Ad ogni ciclo controllo che il parent non sia zero
        // In tal caso push sulla pila

		len = 0;
		while(decompressor->dictionary[index].parent != 0){
            stack_push(s, decompressor->dictionary[index].c);
			index = decompressor -> dictionary[index].parent;
            len++;
		}

        extracted_parent = stack_pop(s);

        // Defensive programming
        if (extracted_parent == EMPTY_STACK){
            printf("Error during decompression: stack is empty and it shouldn't..\n");
            exit(1);
        }

        write_data(&extracted_parent, 1, 1, output_file);

        // Ciclo di estrazione (non vorrei aver esagerato con l'ottimizzazione)
        for (i = 1; i < len; i++){
            extracted_c = stack_pop(s);
            write_data(&extracted_c, 1, 1, output_file);
        }

        // Defensive programming
        if (stack_pop(s) != EMPTY_STACK){
            printf("Error during decompression: stack is not empty\n");
            exit(1);
        }

        // Aggiungo nodo al dizionario
        if (previous_node != ROOT) {
            /** Se vogliamo ottimizzare togliendo la variabile bisogna fare così
             *  decompressor->dictionary[++decompressor->node_count].parent = previous_node;
             *  decompressor->dictionary[decompressor->node_count].c = output_string[0];
             *
             *  (Il male secondo me)
             */
            new_node_count = ++decompressor->node_count;
            decompressor->dictionary[new_node_count].parent = previous_node;
            // extracted_parent "dovrebbe" puntare all'ultimo carattere estratto dalla pila
            // quindi al carattere giusto da aggiungere come ultimo figlio letto
            // (primo nella sequenza di pop)
            decompressor->dictionary[new_node_count].c = extracted_parent;

        }

        // Imposto il nodo ricevuto come prossimo nodo al quale aggiungeremo il figlio
        previous_node = current_node;

        // Dizionario pieno: svuotare tutto
        // FIXME discuterne
        // Secondo me non bisogna proprio fare nulla eccetto il reset del nodecount
        // perchè il dizionario si reinizalizza da solo
        // Sovrascrivendo i dati vecchi mano mano che si procede
        if(decompressor -> node_count == dictionary_size){
            decompressor->node_count = EOF_CODE;
		}

	}

    bitio_close(bitio);
    fclose(output_file);

    bzero(decompressor, sizeof(struct decompressor_data));
    free(decompressor);

    bzero(s->stk, s->size * sizeof(char));
    free(s);

    bzero(header, sizeof(struct file_header));
    free(header);

}





