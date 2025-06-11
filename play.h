#include "curl_read.h"
void play_music(const char* music_url);
char* get_url(char* vidId);
void stop_song();
void pause_song();
void resume_song();
void next_song();
void add_song(out data);
int is_playing();
void init_mpv();
void add_songfile(char* name);