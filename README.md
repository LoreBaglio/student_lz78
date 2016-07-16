# student_lzw
This is a student version of lzw, used for didactic purpose only.
lzw_codec compresses or decompresses a file using LZW algorithm.

lzw_codec -c|-d [-l size][-v] -i filename -o filename

OPTIONS

	-c 
	    compress

	-d
	    decompress

	-l size
	    specify the dictionary size of the compressor

	-v
	    verbose mode

	-i filename
	    specify the input file (the file that will be compressed/decompressed)

	-o filename
	    specify the output file (the file that will contain the result of the compression/decompression)
