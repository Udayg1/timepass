#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dep.h"
#include "curl_read.h"
#include <unistd.h>

out queue[100];
int que_len = 0;
int current_song = -1;

void stop_song();
void reset_queue();
int is_playing();

void init_mpv(){
    // FILE* pipe;
    system("mpv --no-video --really-quiet --no-input-default-bindings --no-input-terminal --idle --input-ipc-server=/tmp/mpvsocket &");
    // pclose(pipe);
}

void play_music(const char* music_url) {
    char command[4096];
    if (!is_playing()){
        sprintf(command, "echo '{\"command\": [\"loadfile\", \"%s\", \"replace\"]}' | socat - /tmp/mpvsocket", music_url);
    }
    else {
        sprintf(command, "echo '{\"command\": [\"loadfile\", \"%s\", \"append\"]}' | socat - /tmp/mpvsocket", music_url);
    }
    FILE* pipe;
    pipe = popen(command, "r");
    usleep(10000);
    pclose(pipe);
}

char* get_url(char* vidId){
    char command[1024];
    char* url =(char*) malloc(4096);
    if (!url){
        printf("Memory allocation failed!");
        exit(1);
    }
    sprintf(command, "yt-dlp -f bestaudio -g https://music.youtube.com/watch?v=%s", vidId);
    FILE *po;
    po = popen(command, "r");
    if (!po){
        printf("Pipe open failed");
        exit(1);
    }
    // char tmp[4096];
    fgets(url, 4096, po);    // FILE* pipe;

    strip(url);
    // sprintf(tmp, "curl -L -A \"Mozilla/5.0 Firefox/134.0\" %s  --output log.txt", url);
    // system(tmp);
    // printf("%s",url);
    pclose(po);
    return url;

}

// void add_song(out data){
//     if (que_len+1 < 100){
//         queue[que_len++] = data;
//         return;
//     }
//     printf("Queue full!");
// }

void next_song(){
    FILE* pipe;
    pipe = popen("echo '{\"command\": [\"playlist-next\"]}' | socat - /tmp/mpvsocket","r");
    usleep(10000);
    pclose(pipe);
}

void stop_song(){
    FILE *pipe;
    pipe = popen("echo '{ \"command\": [\"quit\"] }' | socat - /tmp/mpvsocket", "r");
    usleep(10000);
    pclose(pipe);
}

void pause_song(){
    FILE* pipe;
    pipe = popen("echo '{ \"command\": [\"set_property\", \"pause\", true] }' | socat - /tmp/mpvsocket","r");
    usleep(10000);
    pclose(pipe);
}

void resume_song(){
    FILE* pipe;
    pipe = popen("echo '{ \"command\": [\"set_property\", \"pause\", false] }' | socat - /tmp/mpvsocket","r");
    usleep(10000);
    pclose(pipe);
}

// void currently_playing(int current_song){
//     printf("Song: %s\n", queue[current_song].name);
//     printf("Artist: %s", queue[current_song].artist);
// }

// void reset_queue(){
//     memset(queue, 0, sizeof(queue));
//     que_len = 0;
// }

int is_playing(){
    FILE *pipe;
    char tmp[100];
    pipe = popen("echo '{ \"command\": [\"get_property\", \"idle-active\"] }' | socat - /tmp/mpvsocket","r");
    if (!pipe){
        printf("Error opening pipe in check playing!");
        exit(1);
    }
    fgets(tmp, sizeof(tmp), pipe);
    char* status = slice(tmp, find(tmp, ":")+1, find(tmp, ","));
    strip(status);
    usleep(10000); 
    pclose(pipe);
    return strcmp(status, "true"); 
}