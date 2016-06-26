#include "encode.h"

int write_code(){
    // <azzera uint64>
    // <scrivi e shifta puntatore>
    // <se sfora, scrivere un pezzo e scrivere il resto nel prossimo uint_64>

}

int read_code(){
    // TODO read bits_per_code number of bits and return the conversion in int. The result
    // is a node of the dictionary and will be used by the decompressor
}

int compute_bit_to_represent(int arg){
    return ceil(log(arg));
}

int end_compressed_file(){
    // TODO Chiudere la bitio con params.eof_code e padding
    // RETURN CODE -1 se problemi, 0 se ok
}