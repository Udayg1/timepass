#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dep.h"
#include "curl_read.h"
#include "play.h"
#include<unistd.h>

#define MAX_LINE 1024

void queue_song();
void ytdlp(const char *name, const char *vidId);

int main() {
    char option[10];
    printf("Options: (p)ause, (r)esume, (h)alt, (s)kip, (a)dd a song -> ");
    fgets(option, 10, stdin);
    init_mpv();
    while (1) {
        // if 
        // next_song();
        // is_playing();
        strip(option);
        if (strcmp("s", option) == 0 || strcmp("S", option) == 0){
            next_song();
        }
        else if(strcmp("p", option)== 0 || strcmp("P", option) == 0){
            pause_song();
        }
        else if(strcmp("r", option)== 0 || strcmp("R", option) == 0){
            resume_song();
        }
        else if(strcmp("h", option)== 0 || strcmp("H", option) == 0){
            stop_song();
            exit(0);
        }
        else if(strcmp(option, "a") == 0 || strcmp(option, "A") == 0){
            queue_song();
        }
        else {
            printf("Options: (p)ause, (r)esume, (h)alt, (s)kip, (a)dd a song -> ");
            fgets(option, 10, stdin);
        }
        printf("Options: (p)ause, (r)esume, (h)alt, (s)kip, (a)dd a song -> ");
        fgets(option, 10, stdin);
    }
        

    return 0;
}

void ytdlp(const char *name, const char *vidId) {
    char command[MAX_LINE];
    snprintf(command, sizeof(command),
             "yt-dlp -f bestaudio -q --add-metadata -x -o \"%s.%%(ext)s\" https://music.youtube.com/watch?v=%s",
             name, vidId);
    int status = system(command);
    if (status != 0) {
        printf("An error occurred\n");
    } else {
        printf("Song successfully downloaded\n");
    }
}

void queue_song(){
    char buf[128];
    printf("Enter song and artist name (0 to exit): ");
    fgets(buf, 128, stdin);
    while(1){
        strip(buf);
        if (strlen(buf) == 0){
            printf("Please enter a valid query -> ");
            fgets(buf, sizeof(buf), stdin);
        }
        else if(strcmp (buf, "0") == 0){
            exit(1);
        }
        else {
            size_t len = strlen(buf);
            char ** mid = split(buf, &len, " ");
            char* query = join(mid, len, "+");
            char* webpage = page(query);
            out result = extract_vidId(webpage);
            play_music(get_url(result.vidId));
            break;
        }
    }
}