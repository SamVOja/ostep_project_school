#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void compress(FILE *fp) {
    // compress file and write the compressed contents to standard output (RLE)
    char nchar;
    char prev_char = '\0';
    int count = 1;
    while ((nchar = fgetc(fp)) != EOF) { //get the next character
        if (prev_char == '\0') { //when reading the first character in the file
            prev_char = nchar;
        }
        else if (nchar == prev_char) { // character is same as previous
            count++;
        } 
        else {
            fwrite(&count, sizeof(int32_t), 1, stdout); //write in binary 4 byte integer (32 bits)
            fputc(prev_char, stdout); //write the character
            prev_char = nchar;
            count = 1;
        }
    }
    fwrite(&count, sizeof(int32_t), 1, stdout); 
    fputc(prev_char, stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // no file given as argument
        fprintf(stderr, "my-zip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { // multiple files before ">". compress everything together
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "my-zip: cannot open file\n");
            exit(1);
        }
        compress(fp);
        fclose(fp);
    }

    return 0;
}
