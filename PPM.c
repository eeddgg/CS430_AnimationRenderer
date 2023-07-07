#include "PPM.h"

// writes a PPM header with a given file name
int write_header(uint8_t magic, uint8_t maxval, uint64_t width, uint64_t height, FILE * outputFile){
    return fprintf(outputFile, "P%hhu\n%lu %lu\n%hhu\n", magic, width, height, maxval);

}

// basically just a printf wrapper that writes the pixel for you
int write_p3_pixel(pixel out, FILE * outputFile)
{
    return fprintf(outputFile, "%hhu %hhu %hhu\n", out.red, out.green, out.blue);
}

// writes the raw components of the pixel to the output file, returning write number
int write_p6_pixel(pixel out, FILE * outputFile)
{
    int i=0;
    // writes out the bytes for each color component
    i+=fwrite(&out.red, 1, 1, outputFile);
    i+=fwrite(&out.green, 1, 1, outputFile);
    i+=fwrite(&out.blue, 1,1, outputFile);
    // returns the number of successful writes (<3 indicates write failure)
    return i;
}

// reads in and returns a pixel read from the file
pixel read_p3_pixel(FILE * inputFile){
    pixel newPixel;
    fscanf(inputFile, " %hhu %hhu %hhu", &newPixel.red, &newPixel.green, &newPixel.blue);
    return newPixel;
}

pixel read_p6_pixel(FILE * inputFile){
    // temporary array to hold 1 pixel value
    uint8_t buffer[3];
    // reads in 3 bytes (3 pixels per byte)
    fread(buffer, 3, 1, inputFile);
    // creates pixel from values in buffer
    pixel newPixel={buffer[0], buffer[1], buffer[2]};
    return newPixel;
}

header readHeader(FILE * inputFile){
    header new_header;
    // max length of line is 70
    char buffer[71];
    char strings[4][71];
    // stores the pointer to the location of the # symbol to ignore comments
    char* comment_start;
    char dummy_arr[71];
    char * dummy=dummy_arr;
    int string_count=0;

    // gets 4 valid strings that contain the header
    while(string_count<4)
    {
        fgets(buffer, 71, inputFile);
        //finds and eliminates everything after a comment
        comment_start=strchr(buffer, '#');
        if(comment_start!=NULL){
            comment_start[0]='\0';
        }

        // collects any values on this line
        for(dummy=strtok(buffer, " "); dummy !=NULL; dummy=strtok(NULL," "))
        {
            // copies the individual tokens into strings
            strncpy(strings[string_count], dummy, 71);
            string_count++;

        }

    }
    // parses the strings to make a header
    sscanf(strings[0], " P%hhu", &new_header.magic);
    sscanf(strings[1], " %lu", &new_header.width);
    sscanf(strings[2], " %lu", &new_header.height);
    int check = sscanf(strings[3], " %hhu%c", &new_header.maxval, dummy);
    // checks if a value was read for maxval (fails when too large)
    if(check<1){
        printf("Error: image color depth too large\n");
    }
    // returns a header structure
    return new_header;
}

// reads a p3 image into an array of rgb pixels
pixel * read_p3(FILE * file, header metadata){

    size_t pixel_count=metadata.width*metadata.height;
    pixel * image=(pixel*)malloc(pixel_count*sizeof(pixel));
    int i=0;
    for(i=0; i<pixel_count; i++){
        image[i]=read_p3_pixel(file);
    }
    fclose(file);
    return image;
}

void write_p3(pixel * image, header metadata, char* filename){
    // opens file for writes
    FILE * file=fopen(filename, "w");

    //writes a p3 header
    write_header(3, metadata.maxval, metadata.width, metadata.height,file);

    // calculates number of pixels
    long long pixel_count=metadata.width*metadata.height;
    int i;
    int j = 0;


    for(i=0; i<pixel_count; i++){
        // writes out image in p3 format
        j=write_p3_pixel(image[i], file);
        if(j<1){
            printf("Error: failed to write pixel\n");
            break;
        }
    }
    fclose(file);
}

pixel * read_p6(FILE * file, header metadata){
    size_t pixel_count=metadata.width*metadata.height;
    pixel * image=(pixel*)malloc(pixel_count*sizeof(pixel));
    int i=0;
    for(i=0; i<pixel_count; i++){
        image[i]=read_p6_pixel(file);
    }
    fclose(file);
    return image;
}

void write_p6(pixel * image, header metadata, char* filename){
    FILE * file=fopen(filename, "w");
    // writes a p6 header for the file
    write_header(6, metadata.maxval, metadata.width, metadata.height,file);
    // calculates the necessary number of pixels to write out
    long long pixel_count=metadata.width*metadata.height;
    
    int i;
    int j = 0;
    for(i=0; i<pixel_count; i++){
        // writes out the pixel from the image in a p6 format
        j=write_p6_pixel(image[i], file);
        if(j<3){
            printf("Error: failed to write pixel\n");
            break;
        }
    }
    fclose(file);
}

void write_image(image out, char * filename){
    if(out.metadata.magic==3)
    {
        write_p3(out.pixmap, out.metadata, filename);
    }
    else if (out.metadata.magic==6)
    {
        write_p6(out.pixmap, out.metadata, filename);
    }
    
}

header make_header(uint8_t magic, uint8_t maxval, uint64_t width, uint64_t height){
    header newHeader;
    newHeader.maxval=maxval;
    newHeader.magic=magic;
    newHeader.width=width;
    newHeader.height=height;
    return newHeader;
}

pixel * make_pixmap(header metadata){
    size_t image_size=metadata.width * metadata.height;
    return (pixel *) malloc(sizeof(pixel)*image_size);
}

void free_image(image destroy){
    free(destroy.pixmap);
}

