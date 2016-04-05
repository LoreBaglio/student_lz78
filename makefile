CC=gcc
CFLAGS = -c

all: compressor decompressor lz_command

lz_command: lz_command.o
	$(CC) -o lz_command lz_command.o

lz_command.o: lz_command.c
	$(CC) $(CFLAGS) lz_command.c

compressor: compressor.o
	$(CC) -o compressor compressor.o

compressor.o: compressor.c
	$(CC) $(CFLAGS) compressor.c

decompressor: decompressor.o
	$(CC) -o decompressor decompressor.o

decompressor.o: decompressor.c
	$(CC) $(CFLAGS) decompressor.c

clean:
	rm *.o compressor decompressor lz_command