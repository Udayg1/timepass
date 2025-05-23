#include "dep.h"
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

struct Memory {
    char * data;
    size_t len;
};

static size_t writefunc(void *ptr, size_t size, size_t nmemb, struct Memory *mem) {
    size_t total = size * nmemb;
    char *temp = realloc(mem->data, mem->len + total + 1);
    if (temp == NULL) return 0;
    mem->data = temp;
    memcpy(&(mem->data[mem->len]), ptr, total);
    mem->len += total;
    mem->data[mem->len] = '\0';
    return total;
}

char** extract_vidId(char* file){
    char vidId[128], name[1024];
    int nam = 0, id = 0;
    size_t org_len = strlen(file);
    size_t len = org_len;
    char** data = split(file,&org_len , "\n");
    len = strlen(data[org_len - 1]);
    printf("%zu",len);
    char* temp = data[org_len-1];
    size_t ytindex = find(temp, strlen(temp), "YT");
    temp = slice(temp, ytindex, strlen(temp));
    data = split(temp, &len, "\x22");
    for(int i = 0; i < len; i++){
        if (strcmp(strip(data[i]), "videoId") == 0 && !id){
            strncpy(vidId, data[i+2], 11);
            id = 1;
        }
        if (strcmp(strip(data[i]), "title") == 0 && !nam){
            for (int j = i; j < i+20; j++){
                if (strcmp(data[j], ":") == 0){
                    strncpy(name, data[j+1],strlen(data[j+1]));
                    nam = 1;
                }
            }
        }
    }
    free(data);
    free(temp);
    char **out = (char **) malloc(20);
    out[0] = name, out[1] = vidId, out[2] = NULL;
    return out;
}




char* page(char* query){
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    char url[1000];
    struct Memory chunk = {0};
    snprintf(url, sizeof(url), "https://music.youtube.com/search?q=%s", query);
    if (curl){
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 Firefox/137.0");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, chunk.data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    }
    res = curl_easy_perform(curl);
    if (res != CURLE_OK){
        fprintf(stderr, "Curl failed. Code = %s\n", curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
    return chunk.data;
}