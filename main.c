#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dep.h"
#include "curl_read.h"
#include "play.h"
#include <unistd.h>
#include <pthread.h>

#define MAX_LINE 1024

out data;
void cleanup(void);
void queue_song();
void* ytdlp(void *args);

int main() {
    char option[10];
    printf("Options: (p)ause, (r)esume, (h)alt, (s)kip, set (v)olume#, (a)dd a song -> ");
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
        else if (strcmp("p", option)== 0 || strcmp("P", option) == 0){
            pause_song();
        }
        else if (strcmp("r", option)== 0 || strcmp("R", option) == 0){
            resume_song();
        }
        else if (strcmp("h", option)== 0 || strcmp("H", option) == 0){
            stop_song();
            cleanup();
            break;
        }
        else if (strcmp(option, "a") == 0 || strcmp(option, "A") == 0){
            queue_song();
        }
        else if (option[0] == 'V' || option[0] == 'v'){
            volume(atoi(&option[1]));
        }
        else {
            printf("Options: (p)ause, (r)esume, (h)alt, (s)kip, set (v)olume#, (a)dd a song -> ");
            fgets(option, 10, stdin);
        }
        printf("Options: (p)ause, (r)esume, (h)alt, (s)kip, set (v)olume#, (a)dd a song -> ");
        fgets(option, 10, stdin);
    }
    system("clear");
    return 0;
}

void* ytdlp(void *args) {
    char *name = data.name,  *vidId = data.vidId;
    char command[MAX_LINE];
    snprintf(command, sizeof(command),
             "yt-dlp -f bestaudio -q --add-metadata -x --audio-format opus -o \"%s.%%(ext)s\" https://music.youtube.com/watch?v=%s",
             name, vidId);
    int status = system(command);
    if (status != 0) {
        printf("An error occurred. Song not queued.\n");
    } else {
        char move[1024];
        sprintf(move, "mv \"%s\".opus ./.tmp/", name);
        system(move);
    }
    return NULL;
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
            return;
        }
        else {
            size_t len = strlen(buf);
            char ** mid = split(buf, &len, " ");
            char* query = join(mid, len, "+");
            char* webpage = page(query);
            out result = extract_vidId(webpage);
            if (strcmp(result.name, "exit") == 0){
                return;
            }

            if (is_playing()){
                pthread_t thread;
                data.name = result.name;
                data.artist = result.artist;
                data.vidId = result.vidId;
                pthread_create(&thread, NULL, ytdlp, NULL);  // not very useful here. Simply calling the function would have done the trick. 
                pthread_join(thread, NULL);
                add_songfile(data.name);
                break;
            }
            else {
                play_music(get_url(result.vidId));
                break;
            }
        }
    }
}

