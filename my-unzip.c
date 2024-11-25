#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void decompress(FILE *fp) {
    //write a compressed binary files contents uncompressed to standard output
    int count;
    char ch;

    // while loop keeps working while fread keeps encountering numbers followed by a character
    while (fread(&count, sizeof(int32_t), 1, fp) == 1 && fread(&ch, sizeof(char), 1, fp) == 1) { 
        for (int i = 0; i < count; i++) {
            printf("%c", ch); // print the same single character by the amount specified by count
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // no file given as argument
        fprintf(stderr, "my-unzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { //decompress all given files to standard output
        FILE *fp = fopen(argv[i], "rb"); // read binary file
        if (fp == NULL) {
            fprintf(stderr, "my-unzip: cannot open file\n");
            exit(1);
        }

        decompress(fp);
        fclose(fp);
    }

    return 0;
}
