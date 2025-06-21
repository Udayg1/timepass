#include "curl_read.h"
#include "dep.h"
#include <ctype.h>
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
        // free(chunk.data);
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
    int nam = 0, id = 0, plays = 0, artist = 0, count = 0;
    size_t org_len = strlen(file);
    size_t len = org_len;
    char** data = split(file,&org_len , "\n");
    free(file);
    file = NULL;
    char** list[6];
    char** nee = (char**) malloc(4 * sizeof(char*));
    len = strlen(data[9]);
    // printf("%zu\n",org_len);
    char* temp = strdup(data[9]);
    for (int i = 0; i < org_len; i++){
        free(data[i]);
    }
    free(data);
    data = NULL;
    // printf("%s",temp);
    char* topres = slice(temp ,find(temp, "YT"), strlen(temp));
    size_t toplen = strlen(topres);
    char** top = split(topres, &toplen, "\\x22");
    free(topres);
    topres = NULL;
    for (int i = 0; i < toplen; i++){
        strip(top[i]);
        if (strcmp(top[i], "videoId") == 0 && !id){
            nee[3] = strdup(top[i+2]);  
            id = 1;
        }
        else if(strcmp(top[i], "title") == 0 && !nam){
            for (int j = i; j < i+20; j++) {
                if (strcmp(top[j], ":") == 0){
                    nee[0] = hex_to_char(top[j+1]);
                    nam = 1;
                    break;
                }
            }
        }
        else if (strcmp("•", top[i]) == 0 && !artist){
            nee[1] = hex_to_char(top[i+4]);
            artist = 1;
        }
        if (nam && artist && id){
            break;
        }
    }
    for (int i = 0; i < toplen; i++){
        free(top[i]);
    }
    free(top);
    top = NULL;
    list[count++] = nee;
    nee = (char**) malloc(4 * sizeof(char*));
    nam = artist = plays = id = 0;
    size_t ytindex = find(temp, "title\\x22:\\x7b\\x22runs\\x22:\\x5b\\x7b\\x22text\\x22:\\x22Songs\\x22\\x7d\\x5d\\x7d");
    // printf("%s\n%s", list[0][0],list[0][1]);
    printf("\n\t<Top Result>\nTitle:\t%s\nArtist:\t%s\n", list[0][0], list[0][1]);
    char* duppp = slice(temp, ytindex, strlen(temp));
    len = strlen(duppp);
    char** newdata = split(duppp, &len, "\\x22");
    free(duppp);
    duppp = NULL;
    for (int i = 0; i < len; i++){
        // strip(newdata[i]);
        if (strcmp(newdata[i], "videoId") == 0 && !id){
            nee[3] = newdata[i+2];
            id = 1;
        }
        if (strcmp(newdata[i], "musicResponsiveListItemFlexColumnRenderer") == 0){
            if (!nam) {
                nee[0] = hex_to_char(newdata[i+8]);
                nam = 1;
            }
            else if (!artist) {
                nee[1] = hex_to_char(newdata[i+8]);
                artist = 1;
            }
            else if (!plays) {
                nee[2] = hex_to_char(newdata[i+8]);
                plays = 1;
            }
        }
        if (nam && artist && plays && id) {
            if (strcmp(newdata[i], "MUSIC_RESPONSIVE_LIST_ITEM_HEIGHT_TALL") == 0){
                list[count++] = nee;
                nee = (char**) malloc(4 * sizeof(char*));
                nam = artist = plays = id = 0;
            }
        }
        if (count == 4){
            break;
        }
    }
    free(nee);
    nee = NULL;
    for (int i = 1; i < count; i++){
        printf("\n%d.\t%s\n", i, list[i][0]);
        printf("Artist: %s\n", list[i][1]);
        printf("Plays: \t%s\n", list[i][2]);
    }
    char resp[10];
    printf("\nEnter the choice (1-3, 0 abort, press enter for top result): ");
    fgets(resp, sizeof(resp), stdin);
    strip(resp);
    while (1) {
        if (strcmp(resp,"1") == 0){
            result.name = strdup(list[1][0]);
            result.artist = strdup(list[1][1]);
            result.vidId = strdup(list[1][3]);
            break;
        }
        else if(strcmp(resp,"2") == 0){
            result.name = strdup(list[2][0]);
            result.artist = strdup(list[2][1]);
            result.vidId = strdup(list[2][3]);
            break;
        }
        else if(strcmp(resp,"3") == 0){
            result.name = strdup(list[3][0]);
            result.artist = strdup(list[3][1]);
            result.vidId = strdup(list[3][3]);
            break;
        }
        else if(strcmp(resp,"0") == 0){ 
            result.name = "exit";
            result.vidId = " ";
            result.artist = " ";
            break;
        }
        else if (strcmp("", resp) == 0){
            result.name = strdup(list[0][0]);
            result.artist = strdup(list[0][1]);
            result.vidId = strdup(list[0][3]);
            break;
        }
        else {
            printf("Enter a choice between 1 and 3: ");
            fgets(resp, sizeof(resp),stdin);
            strip(resp);
        }
    }
    for (int i = 0; i < 4; i++){
        free(list[i]);
        }
    for (int i = 0; i < len; i++){
        free(newdata[i]);
    }
    free(newdata);
    newdata = NULL;
    // free(temp);
    return result;
}

char* hex_to_char(const char *str){
    char* result = (char*) malloc(256);
    char* output = result;
    char buf[5];
    while (*str){
        if (*str == '\\'){
            if (*(str+1) == 'x' && isxdigit((char) *(str+2)) && isxdigit((char) *(str+3))){
                buf[0] = *(str + 2);
                buf[1] = *(str + 3);
                buf[2] = '\0';               
                *output++ = (char) strtol(buf, NULL, 16);
                str += 4;
            }
            else if (*(str+1) == 'u' && isxdigit((char) *(str+2)) && isxdigit((char) *(str+3)) && isxdigit((char) *(str+4)) && isxdigit((char) *(str+5))){
                buf[0] = *(str + 2);
                buf[1] = *(str + 3);
                buf[2] = *(str + 4);
                buf[3] = *(str + 5);
                buf[4] = '\0';
                int codepoint = (int)strtol(buf, NULL, 16);
                if (codepoint <= 0x7F) {
                    *output++ = (char)codepoint;
                } 
                else if (codepoint <= 0x7FF) {
                    *output++ = (char)(0xC0 | ((codepoint >> 6) & 0x1F));
                    *output++ = (char)(0x80 | (codepoint & 0x3F));
                } 
                else {
                    // UTF-8 3-byte encoding
                    *output++ = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
                    *output++ = (char)(0x80 | ((codepoint >> 6) & 0x3F));
                    *output++ = (char)(0x80 | (codepoint & 0x3F));
                }
                str += 6;
            }
            else {
            *output++ = *str++;
            }           
        }
        else {
        *output++ = *str++;
        }
    }
    *output = '\0';
    return result;
}