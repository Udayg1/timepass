#ifndef CURL_READ_H
#define CURL_READ_H

typedef struct {
    char *vidId;
    char *name;
} out;
char* hex_to_char(const char* str);
char* page(const char* query);
out extract_vidId(char* html);

#endif