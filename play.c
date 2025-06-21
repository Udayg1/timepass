#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dep.h"
#include <unistd.h>
#include <mpv/client.h>

void stop_song();
void reset_queue();
int is_playing();

struct mpv{
    mpv_handle *hdl;
}init ;

void init_mpv(){
    // FILE* pipe;
    system("mkdir -p .tmp");
    // system("mpv --no-video --really-quiet --no-input-default-bindings --no-input-terminal --idle --input-ipc-server=./.tmp/mpvsocket &");
    // pclose(pipe);
    init.hdl = mpv_create();
    mpv_initialize(init.hdl);
    if (!init.hdl) return;
}

void play_music(const char* music_url) {
    const char *cmd[] = {"loadfile", music_url, "replace", NULL};
    mpv_command(init.hdl, cmd);
}

char* get_url(char* vidId){
    char command[1024];
    // printf("%s", vidId);
    char* url =(char*) malloc(8192);
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
    const char *cmd[] = {"playlist_next", "force",NULL};
    mpv_command(init.hdl, cmd);
}

void stop_song(){
    mpv_command_string(init.hdl, "quit");
}

void pause_song(){
    int pause = 1;
    mpv_set_property(init.hdl, "pause", MPV_FORMAT_FLAG, &pause);
}

void resume_song(){
    int pause = 0;
    mpv_set_property(init.hdl, "pause", MPV_FORMAT_FLAG, &pause);
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
    // FILE *pipe;
    // char tmp[100];
    // pipe = popen("echo '{ \"command\": [\"get_property\", \"idle-active\"] }' | socat - ./.tmp/mpvsocket","r");
    // if (!pipe){
    //     printf("Error opening pipe in check playing!");
    //     exit(1);
    // }
    // fgets(tmp, sizeof(tmp), pipe);
    // char* status = slice(tmp, find(tmp, ":")+1, find(tmp, ","));
    // strip(status);
    // usleep(10000); 
    // pclose(pipe);
    // return strcmp(status, "true"); 
    int idle = 0;
    mpv_get_property(init.hdl, "idle-active", MPV_FORMAT_FLAG, &idle);
    return !idle;
}

void add_songfile(char* name){
    strcat(name, ".opus");
    char final[128];
    sprintf(final, "./.tmp/%s", name);
    const char *cmd[] = {"loadfile", final, "append", NULL};
    mpv_command(init.hdl, cmd);
}

void volume(int vol){
    double value  = (double) vol;
    if (value >= 100){
        mpv_set_property(init.hdl, "volume", MPV_FORMAT_DOUBLE, &(double){100.0});    }
    else if (value < 0){
        mpv_set_property(init.hdl, "volume", MPV_FORMAT_DOUBLE, &(double){0.0});
    }
    else {
        mpv_set_property(init.hdl, "volume", MPV_FORMAT_DOUBLE, &value);
    }
}

void cleanup(void){
    system("rm -rf ./.tmp");
    mpv_terminate_destroy(init.hdl);
}