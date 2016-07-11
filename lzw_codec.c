/**

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
	    explain what is being done

	-i filename
	    specify the input file (the file that will be compressed/decompressed)

	-o filename
	    specify the output file (the file that will contain the result of the compression/decompression)
*/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "compressor.h"
#include "decompressor.h"

#define DEFAULT_DICTIONARY_SIZE 4096
#define MIN_DICTIONARY_SIZE 256
#define MAX_DICTIONARY_SIZE 65536

// Global variable
int verbose_flag = 0;

int main (int argc, char **argv)
{
  int compressor_flag = 0;
  int decompressor_flag = 0;

  int length_flag = 0;
  char *length_value = NULL;
  
  int input_flag = 0;
  char *input_value = NULL;
  
  int output_flag = 0;
  char *output_value = NULL;

  int index;
  int current_option;
  int dictionary_size;

  opterr = 0;

  while ((current_option = getopt (argc, argv, "cdl:i:o:v")) != -1) {
    switch (current_option) {
        case 'c':
        compressor_flag = 1;
        break;

      case 'd':
        decompressor_flag = 1;
        break;

      case 'l':
        length_flag = 1;
        length_value = optarg;
        break;

      case 'i':
        input_flag = 1;
        input_value = optarg;
        break;

      case 'o':
        output_flag = 1;
        output_value = optarg;
        break;

      case 'v':
        verbose_flag = 1;
        break;

      case '?':
        if (optopt == 'i' || optopt == 'o' || optopt == 'l' )
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);

        else if (isprint (optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);

        else
          fprintf(stderr,"Unknown option character `\\x%x'.\n", optopt);
            return 1;

      default:
        exit(1);
    }
  }

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

  if (compressor_flag && decompressor_flag) {
    printf("You cannot compress and decompress at the same time!\n");
    exit(1);
  }


  if (compressor_flag){

    if (verbose_flag)
      printf("Compression phase ----\n");

    if (input_flag && output_flag){

      compare_filenames(input_value, output_value);

      if(length_flag){

      	dictionary_size = atoi(length_value);
       
          if(dictionary_size < MIN_DICTIONARY_SIZE || dictionary_size > MAX_DICTIONARY_SIZE){
            dictionary_size = DEFAULT_DICTIONARY_SIZE;

	    if (verbose_flag)
	      printf("The specified dictionary size is not valid. Default dictionary size will be used.\n");
        }
      }

      else
        dictionary_size = DEFAULT_DICTIONARY_SIZE;

      if (verbose_flag)
        printf("Dictionary size: %d\n",dictionary_size);


      //call compressor
      compress(input_value, output_value, dictionary_size);

    } else {

      if(!input_flag){

        printf("Input file missing\nSpecify the input file with -i option, followed by name:\n -i <filename>\n");
        exit(1);
      }
      else{

        printf("Output file missing\nSpecify the output file with -o option, followed by name:\n -o <filename>\n");
        exit(1);
      }
    }

  } else if (decompressor_flag){

    if (verbose_flag)
      printf("Decompression phase ----\n");

    if (input_flag && output_flag){

      compare_filenames(input_value, output_value);

      if(length_flag){

        if(verbose_flag)

	  printf("You cannot specify the dictionary size during the decompression phase: it will be ignored.\n");
      }

    //call decompressor
    decompress(input_value, output_value);

    } else {

        if(!input_flag){

          printf("Input file missing\nSpecify the input file with -i option, followed by name:\n -i <filename>\n");
          exit(1);
        }
        else{

          printf("Output file missing\nSpecify the output file with -o option, followed by name:\n -o <filename>\n");
          exit(1);
        }
    }

  } else {

    printf("The correct format is:\n./lz_command -c(-d) -i <inputfile> -o <outputfile>\n Optional flags are: \n-l (dictionary size)\n-v (for verbose mode)\n");
  }

  return 0;
}

