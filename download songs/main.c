#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

void search_and_download() {
    char buffer[MAX_LINE];
    char outname[MAX_LINE];
    int name = 0;
    int c,count = 0;
    system("./ytmusic");  // run the python binary
    FILE *file = fopen(".temp","r");
    if (!file) {  
        perror("Failed to open input file");  // check file status, return if error
        return;
    }
    if (!fgets(outname, sizeof(outname), file)) { 
        fprintf(stderr, "Error: Failed to read the song name.\n");
        fclose(file);
        return;
    }
    outname[strcspn(outname, "\n")] = '\0';  // add the null terminator at the new line character position
    if (!fgets(buffer, sizeof(buffer), file)) {
        fprintf(stderr, "Error: Failed to read the YouTube Music ID.\n");
        fclose(file);
        return;
    }
    fclose(file);
    system("rm .temp");  // cleanup the artifacts
    buffer[strcspn(buffer, "\n")] = '\0';
    char command[MAX_LINE];
    snprintf(command, sizeof(command),"yt-dlp -f bestaudio -q --add-metadata -x -o '%s' https://music.youtube.com/watch?v=%s",outname ,buffer);  // download the actual song, rename, extract audio, 
    int status = system(command);
    if (status != 0){  // again check status of download
        printf("An error occured\n");
    }
    else{
        printf("Song successfully downloaded\n");
    }
}

int main() {
    while (1) {
        char choice[4];
        printf("Start a new search? (y/n): ");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;  // Remove the trailing newline

        if (strcmp(choice, "y") == 0) {
            search_and_download();
        } else if (strcmp(choice, "n") == 0) {
            break;
        } else {
            printf("Invalid choice. Please enter 'y' or 'n'.\n");
        }
    }

    return 0;
}
