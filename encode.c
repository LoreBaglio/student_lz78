#include "encode.h"

//TODO controllare gestione errori
//TODO controllare le fread e fwrite

struct bitio{
	FILE* f;
	uint64_t data;
	u_int wp;
	u_int rp;
	u_int mode;
};

struct bitio* bitio_open(FILE* f, u_int mode)
{
	struct bitio* b;
	if(f == NULL || mode > 1){
		errno = EINVAL;
		return NULL;
	}
	b = calloc(1, sizeof(struct bitio));
	if(b == NULL){
		errno = ENOMEM;
		return NULL;
	}
	b->f = f;
	b->mode = mode;

	return b;
}

int bitio_close(struct bitio* b)
{
	int ret = 0;
	if(b == NULL){
		errno = EINVAL;
		return -1;
	}
	if(b->mode == 1 && b->wp > 0){
		if(fwrite ((void*)&b->data, 1, (b->wp + 7)/8, b->f) != 1){
			ret = -1;
		}
	}
	fclose(b->f);
	bzero(b, sizeof(*b));
	free(b);
	return ret;
}

int write_code(struct bitio* b, uint64_t data){
    // <azzera uint64>
    // <scrivi e shifta puntatore>
    // <se sfora, scrivere un pezzo e scrivere il resto nel prossimo uint_64>
	int space;
	if(b == NULL || b->mode != 1){
		errno = EINVAL;
		return -1;
	}
	space = 64 - b->wp;
	data &= (1UL << bits_per_code) - 1;
	if(bits_per_code <= space){
		b->data |= data << b->wp;
		b->wp += bits_per_code;
	}
	else{
		b->data |= data << b->wp;
		if(fwrite((void*)&b->data, 1, 8, b->f) != 1){
			errno = ENOSPC;
			return -1;
		}
		b->data = data >> space;
		b->wp = bits_per_code - space;
	}
	return 0;

}

/*TODO decidere valore di ritorno della read_code:
ora il valore di ritorno indica i bit letti se è un valore positivo >=0 o un errore se <0
*/
int read_code(struct bitio* b, uint64_t* my_data){
    // TODO read bits_per_code number of bits and return the conversion in int. The result
    // is a node of the dictionary and will be used by the decompressor
	int space;
	int ret;
	if(b == NULL || b->mode != 0){
		errno = EINVAL;
		return -1;
	}
	*my_data = 0;
	space = b->wp - b->rp;
	if(bits_per_code <= space){
		*my_data = (b->data >> b->rp)&((1UL << bits_per_code) - 1);
		b->rp += bits_per_code;
	}
	else{
		*my_data = (b->data >> b->rp);
		ret = fread((void*)&b->data, 8, 1, b->f);
		if(ret < 0){
			errno = ENODATA;
			return -1;
		}
		b->wp = ret * 8;
		if(b->wp >= bits_per_code - space){
			*my_data |= b->data << space;
			*my_data &= (1UL << bits_per_code) - 1;
			b->rp = bits_per_code - space;
			return bits_per_code;
		}
		else{
			*my_data |= b->data << space;
			*my_data &= 1UL << (b->wp + space) - 1;
			b->rp = b->wp;
			return b->wp + space;
		}
	}
}

int compute_bit_to_represent(int arg){
    return (int) ceil(log(arg));
}

int end_compressed_file(){
    // TODO Chiudere la bitio con params.eof_code e padding
    // RETURN CODE -1 se problemi, 0 se ok
}
