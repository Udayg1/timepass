#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "dep.h"

int main(int argc, char* argv[]) {
    if (argc != 2){
        printf("Usage: compress {filename}");
        exit(1);
    }
    int size = 16;
    char* data = (char*) malloc(size);
    if (!data){
        printf("Memory allocation failed!");
        exit(1);
    }
    char c;
    size_t len = 0;
    FILE *file;
    file = fopen(argv[1], "r");
    if (!file){
        printf("compress: %s: No such file or directory.", argv[1]);
        exit(1);
    }
    while (( c = getc(file)) != EOF) {
        if (len == size - 1){
            size *= 2;
            data = realloc(data, size);
            if (!data){
                printf("Memory allocation failed!");
                exit(1);
            }
        }
        data[len++] = c;
    }
    data[len] = '\0';
    printf("%s", data);
    free(data);
    size_t qty = hashCode("frzht");
    printf("%zu", qty);
}

char* compress(const char* data){
    size_t len = strlen(data), size = 16;
    char* compressed = (char*) malloc(size);
    if (!compressed){
        printf("Memory allocation failed!");
        exit(1);
    }
    for (int i = 0; i < len; i++){
        
    }
}
