#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 3)
    {
        printf("Usage: colorize infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];
    char *outfile = argv[2];

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 4;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        printf("Could not create %s.\n", outfile);
        return 5;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    printf("The size of bf struct datatypes is %lu\n", sizeof(WORD) + sizeof(WORD) + sizeof(DWORD) + sizeof(WORD) + sizeof(DWORD));
    printf("The size of bf is: %lu\n", sizeof(bf));
    printf("The size of inptr is: %lu\n", sizeof(inptr));
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    // bi in fread means to read each member of bi, in inptr, then store data read from inptr
    // store those data into members of bi.
    BITMAPINFOHEADER bi;
    printf("The size of bi is: %lu\n", sizeof(bi));
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 6;
    }
    // bi and bf  contains meta data info about the image, such as dimension, size....etc, 
    // could use these info to determine how much new memory should be allocated.
    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    // allocate memory for image
    RGBTRIPLE (*image)[width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(outptr);
        fclose(inptr);
        return 7;
    }

    // determine padding for scanlines
    int padding =  (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0; i < height; i++)
    {
        // read row into pixel array
        // i, should contains 3 bytes, 1 bytes for one RBG color, width = how many of them with sizeof(RBGtriple), inptr = source of input
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);

        // skip over padding
        fseek(inptr, padding, SEEK_CUR);
    }

    colorize(height, width, image);
   
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // write new pixels to outfile
    for (int i = 0; i < height; i++)
    {
        // write row to outfile
        fwrite(image[i], sizeof(RGBTRIPLE), width, outptr);

        // write padding at end of row
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    // free memory for image
    free(image);

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    return 0;
}
