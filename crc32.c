
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 * The width of the CRC calculation and result.
 * Modify the typedef for a 16 or 32-bit CRC standard.
 */
typedef uint32_t crc;

#define WIDTH  (8 * sizeof(crc))
#define TOPBIT (1 << (WIDTH - 1))
#define POLYNOMIAL 0xD8  /* 11011 followed by 0's */

crc
crcSlow(uint8_t const message[], int nBytes)
{
    crc  remainder = 0;


    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (int byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder);

}   /* crcSlow() */

// Usage
int main(int argc, char* argv[]){

    unsigned char* testCrc = strlen(sizeof(argv[1]));
    strcpy(testCrc, argv[1]);

    uint8_t* text = (uint8_t *) testCrc;

    crc ret = crcSlow(text, sizeof(text));
    printf("Il crc e' : %d\n", ret);
    return 0;
}
