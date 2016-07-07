
#include "encode.h"

struct bitio* bitio_open(const char* filename, u_int mode)
{
	struct bitio* b;
	if(filename == NULL || filename[0] == '\0' || mode > 1){
		errno = EINVAL;
		return NULL;
	}

	b = calloc(1, sizeof(struct bitio));
	if(b == NULL){
		errno = ENOMEM;
		return NULL;
	}

	// Init global var
	size_bitio_block = sizeof(b->data);

	b->f = fopen(filename, (mode == 0) ? "r" : "w");

	if (b->f == NULL){
		free(b);
		return NULL;
	}

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

int compressor_bitio_close(struct bitio* b, unsigned char* content, off_t original_size, int header_size)
{
	int ret = 0;
	uint8_t is_compressed;

	if(b == NULL){
		errno = EINVAL;
		return -1;
	}
	if(b->mode == 1 && b->wp > 0){
		if(fwrite ((void*)&b->data, 1, (b->wp + 7)/8, b->f) != 1){
			ret = -1;
		}
	}

	is_compressed =  check_size(b->f, original_size, header_size);
	fseek(b->f, header_size - sizeof(uint8_t), SEEK_SET);
    	if (fwrite(&is_compressed, sizeof(uint8_t), 1, b->f) != 1){
		ret = -1;
	}
	if(is_compressed == 0){
		if (fwrite(content, original_size, 1, b->f) != 1){
			ret = -1;
		}
	}
	fclose(b->f);
	bzero(b, sizeof(*b));
	free(b);
	return ret;
}

int write_code(struct bitio* b, int size, uint64_t data){
    // <azzera uint64>
    // <scrivi e shifta puntatore>
    // <se sfora, scrivere un pezzo e scrivere il resto nel prossimo uint_64>
	int space, ret;
	if(b == NULL || b->mode != 1 || size > 64){
		errno = EINVAL;
		return -1;
	}
	if(size == 0){
		return 0;
	}
	space = 64 - b->wp;
	data &= (1UL << size) - 1;
	if(size <= space){
		b->data |= data << b->wp;
		b->wp += size;
	}
	else{
        // altrimenti se è 64 il compilatore ignora lo shift in quanto è uno shift rotante, i bit non vengono persi
		if(b->wp < 64) b->data |= data << b->wp;
		ret = fwrite((void*)&b->data, 1, 8, b->f);
		if( ret != 8){
			errno = ENOSPC;
			return -1;
		}
		b->data = data >> space;
		b->wp = size - space;
	}
	return 0;

}

/*
    Return number of read bits if positive, error if negative
*/
int read_code(struct bitio* b, int size, uint64_t* my_data){
	int space;
	int ret;
	if(b == NULL || b->mode != 0 || size > 64){
		errno = EINVAL;
		return -1;
	}
	if(size == 0){
    		return 0;
    	}
	*my_data = 0;
	space = b->wp - b->rp;
	if(size <= space){
		*my_data = (b->data >> b->rp)&((1UL << size) - 1);
		b->rp += size;
		return size;
	}
	else{
	    // altrimenti se è 64 il compilatore ignora lo shift in quanto è uno shift rotante, i bit non vengono persi
		if(b->rp < 64) *my_data = (b->data >> b->rp);
		ret = fread((void*)&b->data, 1, 8, b->f);
		if(ret < 0){
			errno = ENODATA;
			return -1;
		}
		b->wp = ret * 8;
		if(b->wp >= size - space){
			*my_data |= b->data << space;
			*my_data &= (1UL << size) - 1;
			b->rp = size - space;
			return size;
		}
		else{
			*my_data |= b->data << space;
			*my_data &= 1UL << (b->wp + space) - 1;
			b->rp = b->wp;
			return b->wp + space;
		}
	}
}

u_int compute_bit_to_represent(int arg){
    
    return (u_int) ceil(log(arg) / log(2));
   
}

int end_compressed_file(){
    // TODO Chiudere la bitio con params.eof_code e padding
    // RETURN CODE -1 se problemi, 0 se ok
}
