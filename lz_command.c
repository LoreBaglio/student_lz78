#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global variables

int verbose_flag = 0;

// Support functions

void print_verbose(const char* output){
  if (verbose_flag)
    printf(output);
}



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
  int c;

  opterr = 0;

  while ((c = getopt (argc, argv, "cdl:i:o:v")) != -1) {
    switch (c) {
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
          fprintf(stderr,
                  "Unknown option character `\\x%x'.\n",
                  optopt);
            return 1;

      default:
        exit(1);
    }
  }

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

  //TODO Scegliere se l'opzione -l è obbligatoria

  if (compressor_flag && decompressor_flag) {
    printf("You cannot compress and decompress at the same time!\n");
    exit(1);
  }


  if (compressor_flag){

    print_verbose("Compression phase ----\n");

    if (input_flag){
      //TODO Chiamata compressore
    } else {
      printf("Input file missing\nSpecify the input file with -i option, followed by name:\n"
                     "-i <filename>\n");
      exit(1);
    }

  } else if (decompressor_flag){

    print_verbose("Decompression phase ----\n");

    if (input_flag){
      //TODO Chiamata decompressore
    } else {
      printf("Input file missing\nSpecify the input file with -i option, followed by name:\n"
                     "-i <filename>\n");
      exit(1);

    }

  } else {

    printf("The correct format is:\n./lz_command -c(-d) -i <inputfile> -o <outputfile>\n"
                   "Optional flags are: \n-l (dictionary size)\n-v (for verbose mode)\n");
  }

  return 0;
}

