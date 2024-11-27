#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <stdatomic.h>

typedef struct { // structure holds arguments for each thread
    char *data;             
    size_t i;           
    size_t file_size;      
    uint8_t *output_int;    
    uint8_t *output_char;   
    atomic_size_t *output_int_size; 
    atomic_size_t *output_char_size; 
    pthread_mutex_t *mutex; 
} ThreadArgs;

void *compare(void *arg) {
    // compare a single character to the previous character from an input and determine if its the same
    ThreadArgs *args = (ThreadArgs *)arg;

    pthread_mutex_lock(args->mutex); //lock

    if (args->i == 0 || args->data[args->i] != args->data[args->i - 1]) {
        //if first character or a different character than previous
        int32_t count = 1; 

        size_t pos = atomic_fetch_add(args->output_int_size, sizeof(int32_t));
        if (pos + sizeof(int32_t) <= args->file_size * sizeof(int32_t)) {
            memcpy(args->output_int + pos, &count, sizeof(int32_t));
        } // store count

        size_t char_pos = atomic_fetch_add(args->output_char_size, 1);
        if (char_pos < args->file_size) {
            args->output_char[char_pos] = args->data[args->i];
        } // store character

    } else { //if same as previous character
        size_t pos = atomic_load(args->output_int_size) - sizeof(int32_t);
        if (pos < args->file_size * sizeof(int32_t)) {
            int32_t *count = (int32_t *)(args->output_int + pos);
            (*count)++; // just increase count
        }
    }

    pthread_mutex_unlock(args->mutex);
    return NULL;
}

void parallel_compress(FILE *fp, int n_threads, size_t file_size) {
    // compress given file using RLE with parallel threads
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    atomic_size_t output_int_size = 0;
    atomic_size_t output_char_size = 0;
    uint8_t *output_int = malloc(file_size * sizeof(int32_t));
    uint8_t *output_char = malloc(file_size);
    char *data = malloc(file_size);
    pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
    ThreadArgs *thread_args = malloc(n_threads * sizeof(ThreadArgs));

    if (!output_int || !output_char || !data || !threads || !thread_args) {
        perror("Error allocating memory");
        exit(1);
    }

    size_t bytes_read = fread(data, 1, file_size, fp);
    if (bytes_read != file_size) {
        fprintf(stderr, "Error reading file\n");
        free(data);
        free(output_int);
        free(output_char);
        exit(1);
    }

    for (size_t i = 0; i < file_size; i++) {
        size_t active_thread = i % n_threads; //threads actually in use

        thread_args[active_thread] = (ThreadArgs){
            .data = data,
            .i = i,
            .file_size = file_size,
            .output_int = output_int,
            .output_char = output_char,
            .output_int_size = &output_int_size,
            .output_char_size = &output_char_size,
            .mutex = &mutex,
        };

        pthread_create(&threads[active_thread], NULL, compare, &thread_args[active_thread]);

        if (active_thread == n_threads - 1 || i == file_size - 1) {
            for (size_t j = 0; j <= active_thread; j++) {
                pthread_join(threads[j], NULL);
            }
        }
    }

    //output
    for (size_t i = 0; i < atomic_load(&output_int_size) / sizeof(int32_t); i++) {
        int32_t count = *((int32_t *)(output_int + i * sizeof(int32_t)));
        char current_char = output_char[i];
        fwrite(&count, sizeof(int32_t), 1, stdout); // write a raw 4-byte integer
        fwrite(&current_char, sizeof(char), 1, stdout); // followed by a single character
    }

    free(threads);
    free(thread_args);
    free(data);
    free(output_int);
    free(output_char);
}

int main(int argc, char *argv[]) {
    if (argc < 2) { // no file given as argument
        fprintf(stderr, "pzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) { // compress given files one by one. compress everything together
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            fprintf(stderr, "pzip: cannot open file\n");
            exit(1);
        }
        struct stat st;
        stat(argv[i], &st);
        size_t file_size = st.st_size;
        parallel_compress(fp, get_nprocs(), file_size); // get_nprocs() for the number of threads
        fclose(fp);
    }
    return 0;
}
