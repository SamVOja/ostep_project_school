#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node { //linked list structure
    char *line;
    struct Node *next;
} Node;

void stdout_reverse(Node *head) {
    // write lines stored in a linked list to standard output in reverse
    if (head) { //if there is still nodes left
        stdout_reverse(head->next); 
        printf("%s\n", head->line); 
    }
}

void write_to_file_reverse(FILE *file, Node *head) {
    // write lines stored in a linked list to an output file in reverse
    if (head) { //if there is still nodes left
        write_to_file_reverse(file, head->next); 
        fprintf(file, "%s\n", head->line);
    }
}

Node *read_from_stdin() {
    // read all lines from standard input and store them in a linked list
    Node *head = NULL;
    Node *tail = NULL;
    size_t line_size = 0;
    char *line = NULL;

    while (getline(&line, &line_size, stdin) != -1) {
        //getline dynamically allocates the buffer size
        size_t len = strlen(line);
        // if line ends with a newline, remove the newline
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        Node *new_node = malloc(sizeof(Node));
        if (!new_node) {
            fprintf(stderr, "malloc failed\n");
            free(line);
            exit(1);
        }
        new_node->line = line; //store the line
        new_node->next = NULL; //initialize next node

        if (!head) {
            head = new_node; //first node in the linked list
        } else {
            tail->next = new_node; //add to the end of the linked list
        }
        tail = new_node;
        line = NULL;
    }

    free(line);
    return head;
}

Node *read_from_file(const char *filename) {
    // read all lines from input file and store them in a linked list
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "error: cannot open file '%s'\n", filename);
        exit(1);
    }

    Node *head = NULL;
    Node *tail = NULL;
    size_t line_size = 0;
    char *line = NULL;

    while (getline(&line, &line_size, file) != -1) {
        //getline dynamically allocates the buffer size
        size_t len = strlen(line);
        // if line ends with a newline, remove the newline
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        Node *new_node = malloc(sizeof(Node));
        if (!new_node) {
            fprintf(stderr, "malloc failed\n");
            free(line);
            fclose(file);
            exit(1);
        }
        new_node->line = line; //store the line
        new_node->next = NULL; //initialize next node

        if (!head) {
            head = new_node; //first node in the linked list
        } else {
            tail->next = new_node;  //add to the end of the linked list
        }
        tail = new_node; 
        line = NULL; 
    }
    free(line); 
    fclose(file);
    return head;
}

void free_memory(Node *head) {
    // empty the linked list
    while (head) { //if there is still nodes left
        Node *temp = head;
        head = head->next;
        free(temp->line);
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    Node *lines = NULL;

    if (argc == 1) { // 0 arguments: read from stdin, write to stdout
        lines = read_from_stdin();
        stdout_reverse(lines);

    } else if (argc == 2) { // 1 argument: read input file, write to stdout
        lines = read_from_file(argv[1]);
        stdout_reverse(lines);

    } else if (argc == 3) { // 2 arguments: read input file, write to output file
        if (strcmp(argv[2], argv[1]) == 0) {
            fprintf(stderr, "Input and output file must differ\n");
            exit(1);
        }
        FILE *file = fopen(argv[2], "w");
        if (!file) {
            fprintf(stderr, "error: cannot open file '%s'\n", argv[2]);
            exit(1);
        }
        lines = read_from_file(argv[1]);
        write_to_file_reverse(file, lines); 
        fclose(file);

    } else { // argc > 3, too many arguments
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if (lines) {
        free_memory(lines);
    }

    return 0;
}
