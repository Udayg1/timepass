#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEP
char* slice(char* arr, int start, int end);

char** split(char* arr, size_t *size, char* spl){
    int cap = 128;
    char** out = (char**) malloc(sizeof(char*) * cap);
    char* temp = (char*)malloc(strlen(arr)+1);  // allocate memory enough to hold the complete string. (no spl char found in case)
    int count_out = 0, count_temp = 0;
    for (int i = 0; i < strlen(arr) - strlen(spl); i++){
        if (strncmp(&arr[i], spl, strlen(spl)) == 0 || arr[i] == '\0'){
            i = i + (strlen(spl)-1);
            temp[count_temp] = '\0';
            if (count_out + 1 > cap ){
                cap *= 2;
                char **temp_out = (char **) realloc(out, sizeof(char*) * cap);  // add memory if overflowing. 
                if (temp_out == NULL){
                    fprintf(stderr, "Memory allocation failed!\n");
                    exit(1);
                }
                out = temp_out;
            }
            out[count_out] = (char*) malloc(count_temp + 1);  // allocate memory to each substring
            strcpy(out[count_out], temp);
            count_out += 1;
            count_temp = 0;
            memset(temp, 0, sizeof temp);  // set the buffer array to 0 
        }
        else {         
            temp[count_temp++] = arr[i];
        }
    }
    // out[count_out] = (char*) malloc(count_temp + 1);  // append remainging 
    // strcpy(out[count_out++], temp);
    out[count_out] = NULL;
    free(temp);
    *size = count_out;
    return out;
}

size_t find(char* arr, char* str){  // return first occurence of str in the arr. 
    size_t len = strlen(str);
    for (int i = 0; i < strlen(arr) - len; i++){
        int match = 1;
        for (int j = 0; j < len; j++){
            if(str[j] != arr[i+j]){  // compare word by word of the substring. 
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

char* slice(char* arr, int start, int end){  // slice the string starting from start upto and including end -1. return upto the end if end > length of string. 
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
        if (!isspace(arr[index])){  // traverse from end and put null after first non white space encountered
            arr[index+1]='\0';
            break;
        }
    }
    int start = 0;
    while (arr[start] && isspace((unsigned char)arr[start])) {  // start from the beginning and add move the memory from first non white char 
        start++;
    }
    if (start > 0) {
        memmove(arr, arr + start, strlen(arr + start) + 1);  // +1 to copy the null terminator
    }
}

char* join(char** arr, size_t len, const char* delimiter){
    size_t tlen = 0;
    for (int i = 0; arr[i] != NULL; i++){  // calculate total length to allocate to the output string
        tlen += strlen(arr[i]) + strlen(delimiter);
    }
    char* out = (char*)malloc(tlen+1);
    out[0] = '\0';  // strcat require this to find where the string ends.
    for (size_t i = 0; arr[i] != NULL; i++){
        strcat(out, arr[i]);
        if (i < len-1){
            strcat(out, delimiter);  // add data and delimiter except the last iteration. 
        }
    }
    return out;
}



#endif