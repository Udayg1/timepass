#include "curl_read.h"
#include "dep.h"
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

struct Memory {
    char *data;
    size_t size;
};


// Callback function to write data into memory buffer
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *ptr = realloc(mem->data, mem->size + real_size + 1);
    if (ptr == NULL) {
        // Out of memory!
        fprintf(stderr, "realloc() failed\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->data[mem->size] = '\0';

    return real_size;
}

// Function to fetch URL content as string
char* page(const char *query) {
    CURL *curl;
    CURLcode res;

    struct Memory chunk;
    chunk.data = malloc(1);  // will be grown as needed by realloc
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        free(chunk.data);
        return NULL;
    }
    char url[1024];
    sprintf(url, "https://music.youtube.com/search?q=%s", query);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT,
        "Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/123.0.0.0 Safari/537.36");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        free(chunk.data);
        chunk.data = NULL;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return chunk.data;  // caller must free()
}

out extract_vidId(char* file){
    out result;
    int nam = 0, id = 0;
    size_t org_len = strlen(file);
    size_t len = org_len;
    char** data = split(file,&org_len , "\n");
    len = strlen(data[9]);
    // printf("%zu\n",org_len);
    char* temp = data[9];
    // printf("%s",temp);
    size_t ytindex = find(temp, "Songs");
    temp = slice(temp, ytindex, strlen(temp));
    len = strlen(temp);
    data = split(temp, &len, "\\x22");
    for(int i = 0; i < len; i++){
        // strip(data[i]);
        if (strcmp(data[i], "videoId") == 0 && !id){
            result.vidId = data[i+2];
            id = 1;
        }
        if (strcmp(data[i], "musicResponsiveListItemFlexColumnRenderer") == 0 && !nam){
            for (int j = i; j < i+20; j++){
                if (strcmp(data[j], ":") == 0 && !nam){
                    result.name = data[j+1];
                    nam = 1;
                }
            }
        }
    }
    // free(data);
    // free(temp);
    // char **out = (char **) malloc(5 * sizeof(char*));
    // *name[strlen(*name)] = '\0';
    // *vidId[strlen(*vidId)] = '\0';
    // printf("%s", *name);
    // printf("%s", *vidId);
    return result;
}

