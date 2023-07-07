#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdint.h>
#include<ctype.h>

typedef struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}pixel;

typedef struct header{
    uint8_t magic;
    uint8_t maxval;
    uint64_t height;
    uint64_t width;
} header;

typedef struct image{
    header metadata; 
    pixel * pixmap;
} image;

int write_header(uint8_t magic, uint8_t maxval, uint64_t width, uint64_t height, FILE * outputFile);
int write_p3_pixel(pixel out, FILE * outputFile);
int write_p6_pixel(pixel out, FILE * outputFile);
pixel read_p3_pixel(FILE * inputFile);
pixel read_p6_pixel(FILE * inputFile);
header readHeader(FILE * inputFile);
pixel * read_p3(FILE * file, header metadata);
void write_p3(pixel * image, header metadata, char* filename);
pixel * read_p6(FILE * file, header metadata);
void write_p6(pixel * image, header metadata, char* filename);
void write_image(image out, char * filename);
header make_header(uint8_t magic, uint8_t maxval, uint64_t width, uint64_t height);
pixel * make_pixmap(header metadata);
void free_image(image destroy);