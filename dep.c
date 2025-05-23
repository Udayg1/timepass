#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEP
char* slice(char* arr, int start, int end);

char** split(char* arr, size_t *size, char* spl){
    int cap = 128;
    char** out = (char**) malloc(sizeof(char*) * cap);
    char* temp = (char*)malloc(strlen(arr)+1);
    int count_out = 0, count_temp = 0;
    for (int i = 0; i < strlen(arr) - strlen(spl); i++){
        if (strncmp(&arr[i], spl, strlen(spl)) == 0 || arr[i] == '\0'){
            i = i + (strlen(spl)-1);
            temp[count_temp] = '\0';
            if (count_out + 1 > cap ){
                cap *= 2;
                char **temp_out = (char **) realloc(out, sizeof(char*) * cap);
                if (temp_out == NULL){
                    fprintf(stderr, "Memory allocation failed!\n");
                    exit(1);
                }
                out = temp_out;
            }
            out[count_out] = (char*) malloc(count_temp + 1);
            strcpy(out[count_out], temp);
            count_out += 1;
            count_temp = 0;
        }
        else {         
            temp[count_temp++] = arr[i];
        }
    }
    // out[count_out] = (char*) malloc(count_temp + 1);
    // strcpy(out[count_out++], temp);
    out[count_out] = NULL;
    free(temp);
    *size = count_out;
    return out;
}

size_t find(char* arr, int size, char* str){
    size_t len = strlen(str);
    for (int i = 0; i <= size - len; i++){
        int match = 1;
        for (int j = 0; j < len; j++){
            if(str[j] != arr[i+j]){
                match = 0;
                break;
            }
        }
        if (match){
            return i;
        }
    }
    return len;
}

char* slice(char* arr, int start, int end){
    if (start < 0 || end < 0){
        perror("Cant slice negative index");
        exit(1);
    }
    if (end < start){
        perror("End index must be greater than start index");
        exit(1);
    }
    if (end > strlen(arr)){
        end = strlen(arr);
    }
    char* out = (char*) malloc(end - start + 10);
    int count = 0;
    for (int i = start; i < end; i++){
        out[count++] = arr[i];
    }
    out[count] = '\0';
    return out;
}

void strip(char* arr){
    int len = strlen(arr);
    for (int index = len-1; index >= 0; index--){
        if (!isspace(arr[index])){
            arr[index+1]='\0';
            break;
        }
    }
    int start = 0;
    while (arr[start] && isspace((unsigned char)arr[start])) {
        start++;
    }
    if (start > 0) {
        memmove(arr, arr + start, strlen(arr + start) + 1);  // +1 to copy the null terminator
    }
}

char* join(char** arr, size_t len, const char* delimiter){
    size_t tlen = 0;
    for (int i = 0; i < len; i++){
        tlen += strlen(arr[i]);
    }
    char* out = (char*)malloc(tlen+1);
    for (size_t i = 0; i < len; i++){
        strcat(out, arr[i]);
        if (i < len-1){
            strcat(out, delimiter);
        }
    }
    return out;
}



#endif