CC=gcc
SOURCES = lzw_codec.c compressor.c decompressor.c hash_table.c file_io.c encode.c 
OBJECTS = $(SOURCES:.c=.o)
LIBS =  -lm

all: lzw_codec

lzw_codec: $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm lzw_codec $(objects)
