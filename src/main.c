#include "./headers/playing.h"
#include "./headers/playlist.h"
#include "./headers/read_platlist.h"
#include "./headers/audio.h"
#include "./headers/helpers.h"
#include <stdio.h>
#include <string.h>
int main(){
    sqlite3 *db = NULL;
    int rc;
    char *q1 = "Do you want to play song y/n?\n";
    char an1 = get_user_input(q1);
    clear_input_buffer();
    if(an1 == 'y' || an1 == 'Y'){
       char file[256];
        printf("Give path to file: ");
        if (fgets(file, sizeof(file), stdin) != NULL) {
            file[strcspn(file, "\n")] = 0; // Remove newline
            FILE *song_check = CheckTheFile(file);
            if (song_check) {
                int correct = 0;
                PlayingSong(song_check, &correct);
                if (correct != 0) {
                    fprintf(stderr, "Playing went wrong\n");
                }
                fclose(song_check);
            }
        }
    }
    char *q2 = "Do you want to create a playlist y/n";
    char an2 = get_user_input(q2);
    clear_input_buffer();
    if (an2 == 'y' || an2 == 'Y') {
        db = OpenDB(&rc);
        if (db == NULL) {
            fprintf(stderr, "Failed to open database\n");
            return 1;
        }
        char dbName[256];
        int con = true;
        printf("What name do you want for it /n");
        if(fgets(dbName, sizeof(dbName),stdin) != NULL){
            int id_playlist = -1;
            clear_input_buffer();
            createPlaylist(db,dbName,&id_playlist);
            while(con){
                char song[256];
                printf("past the file adress of the song \n");
                if(fgets(song, sizeof(song),stdin) != NULL){
                    clear_input_buffer();
                  song[strcspn(song, "\n")] = 0;
                }else {
                fprintf(stdout,"wrong path");
                    continue;
                }
                FILE *song_check = CheckTheFile(song);
                if(!song_check){
                    printf("wrong path");
                    continue;
                }
            const char *name = getName(song);
            printf("name of song: %s \n",name);
           if(insertPlaylist(song,db,name) != 0){
                fprintf(stderr, "Failed to insert song into playlist\n");
            }
            clear_input_buffer();
            char *q3 = "do you want to add more songs? y \n";
            char an3 = get_user_input(q3);
            if(an3 == 'n' || an3 == 'N'){
                con = false;
            }
            clear_input_buffer();
        }
        sqlite3_close(db);
    }
    }
    char *q4 = "Do you want to start playing from playlist  y \n";
    char an4 = get_user_input(q4);
    clear_input_buffer();
    if (an4 == 'y' || an4 == 'Y') {
        db = OpenDB(&rc);
        if (db == NULL) {
            fprintf(stderr, "Failed to open database\n");
            return 1;
        }
        printf("what is name of playlist \n");
        char an5[256];
        printf("what is name of playlist");
        if(fgets(an5,sizeof(an5) ,stdin) != NULL){
            an5[strcspn(an5,"\n")] = 0;
            int id = finding_playlist(db,an5);
            if(id != -1){
                SongNode *head = reading_in_order(db,id);
                while(head != NULL){
                    FILE *song_check = CheckTheFile(head->path);
                    if (song_check) {
                        int correct = 0;
                        PlayingSong(song_check, &correct);
                        if (correct != 0) {
                           fprintf(stderr, "Failed playing song: %s\n", head->name);
                        }
                        fclose(song_check);
                    }
                    head = head->next;
            }
        }
    }
        }
return 0;
}


