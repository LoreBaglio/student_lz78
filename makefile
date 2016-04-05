CC=gcc
CFLAGS = -c

all: compressor decompressor

compressor: compressor.o
	$(CC) -o compressor compressor.o

compressor.o: compressor.c
	$(CC) $(CFLAGS) compressor.c

decompressor: decompressor.o
	$(CC) -o decompressor decompressor.o

decompressor.o: decompressor.c
	$(CC) $(CFLAGS) decompressor.c

clean:
	rm *.o compressor decompressor
