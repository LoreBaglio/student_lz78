/**
calcolo del checksum con l'algoritmo md5
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<openssl/md5.h>
#include<sys/stat.h>
#include<openssl/evp.h>

void print_bytes(const unsigned char * buf, int len)
{
	int i;
	for(i = 0; i < len - 1; i++){
		printf("%02X:", buf[i]);
	}
	printf("%02X", buf[i]);
}

int main(int argc, char* argv[])
{
	FILE* fp;
	unsigned char* text;
	int size;
	struct stat file_stat;
	int ret;

	unsigned char* digest;
	int digest_len = 0;

	MD5_CTX* c;

	EVP_MD_CTX* md_ctx;	

	ret = stat(argv[1], &file_stat);
	if(ret < 0){
		printf("error in stat()\n");
		exit(1);
	}
	size = file_stat.st_size;

	fp = fopen(argv[1], "r");
	if(fp == NULL){
		printf("error in fopen()\n");
		exit(1);
	}
	
	text = (unsigned char*)malloc(size);
	if(fread(text, 1, size, fp) < size){
		printf("error in fread()\n");
		exit(1);
	}
	text[size] = '\0';
	fclose(fp);

	//MD5()
	digest = (unsigned char*)malloc(MD5_DIGEST_LENGTH);
	MD5(text, size, digest);	

	printf("checksum: \t");
	print_bytes(digest, MD5_DIGEST_LENGTH);
	printf("\n");
	free(digest);	

	// a blocchi con MD5_CTX()
	digest = (unsigned char*)malloc(MD5_DIGEST_LENGTH);
	c = (MD5_CTX*)malloc(sizeof(MD5_CTX));
	MD5_Init(c);
	MD5_Update(c, text, size);
	MD5_Final(digest, c);

	printf("checksum: \t");
	print_bytes(digest, MD5_DIGEST_LENGTH);
	printf("\n");
	free(digest);

	//EVP Hashing Context
	digest = (unsigned char*)malloc(EVP_MD_size(EVP_md5()));
	md_ctx = malloc(sizeof(EVP_MD_CTX));
	EVP_MD_CTX_init(md_ctx);
	EVP_DigestInit(md_ctx, EVP_md5());
	EVP_DigestUpdate(md_ctx, text, size);
	EVP_DigestFinal(md_ctx, digest, &digest_len);	
	EVP_MD_CTX_cleanup(md_ctx);
	free(md_ctx);

	printf("checksum: \t");
	print_bytes(digest, EVP_MD_size(EVP_md5()));
	printf("\n");
	free(digest);
	
	free(text);
	free(c);

	return 0;
}
