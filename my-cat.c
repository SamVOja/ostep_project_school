#include <stdio.h>
#include <stdlib.h>

void print_file(const char *filename) {
    // function which writes given files contents on the standard output
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "my-cat: cannot open file\n");
        exit(1);
    }

    char buffer[1024]; // buffer size, how many characters per line can be stored
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer); //print line by line
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc == 1) { // no files given, exit successfully
        return 0;
    }

    for (int i = 1; i < argc; i++) { // loop through each file given
        print_file(argv[i]);
    }
    return 0; 
}
