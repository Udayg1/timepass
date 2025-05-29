#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dep.h"
#include "curl_read.h"

#define MAX_LINE 1024

void ytdlp(const char *name, const char *vidId);

int main() {
    while (1) {
        char choice[4];
        printf("Start a new search? (y/n): ");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "y") == 0) {
            char buffer[128];
            printf("Enter the song and artist name: ");
            fgets(buffer, sizeof(buffer), stdin);
            strip(buffer);
            size_t len = strlen(buffer);
            char **data = split(buffer,&len," ");
            char *new = join(split(buffer, &len, " "), len, "+");
            char * raw = page(new);
            // printf("%s",raw);
            out result = extract_vidId(raw);
            if (strcmp(result.name, "exit") == 0){
                exit(1);
            }
            ytdlp(result.name, result.vidId);
        } else if (strcmp(choice, "n") == 0) {
            break;
        } else {
            printf("Invalid choice. Please enter 'y' or 'n'.\n");
        }
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