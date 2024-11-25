#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void search_lines(const char *target, FILE *file) {
    // write to standard output all lines in a file which include the specified term
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        if (strstr(line, target) != NULL) { // check for the string in a line, case-sensitive
            printf("%s\n", line);
        }
    }
    free(line); 
}

void search_lines_stdin(const char *target) {
    // write to standard output all lines in standard input which include the specified term
    char **lines = NULL; 
    char *line = NULL;  
    size_t count = 0;  
    size_t len = 0;     

    while (getline(&line, &len, stdin) != -1) {
        // store the stdin lines to memory because otherwise they are instantly printed, not after stdin is closed
        lines = realloc(lines, sizeof(char *) * (count + 1));
        if (!lines) {
            perror("memory error");
            free(line);
            exit(1);
        }
        lines[count++] = strdup(line); 
    }
    free(line);

    for (size_t i = 0; i < count; i++) {
        if (strstr(lines[i], target) != NULL) { // check for the string in a line, case-sensitive
            printf("%s\n", lines[i]);
        }
        free(lines[i]); // clear the memory while iterating through the stored lines
    }
    free(lines);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // no search term given
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    if (strlen(argv[1]) == 0) { // if search term is empty, return (alternatively we could match all lines)
        return 0;
    }

    if (argc == 2) { // read standard input
        search_lines_stdin(argv[1]);

    } else { // read one or more files
        for (int i = 2; i < argc; i++) {
            FILE *fp = fopen(argv[i], "r");
            if (!fp) {
                fprintf(stderr, "my-grep: cannot open file\n");
                exit(1);
            }

            search_lines(argv[1], fp);
            fclose(fp);
        }
    }

    return 0;
}
