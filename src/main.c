#include "./headers/playing.h"
#include "./headers/playlist.h"
#include "./headers/audio.h"
int main(){
      char q1;
    printf("Do you want to play song y/n?\n");
    scanf("%c",&q1);
    while ((q1 = getchar()) != '\n' && q1 != EOF) { }
    if(q1 == 'y' || q1 == 'Y'){
        char file[256];
        printf("Give path to file: ");
        scanf("%255s", file);
        FILE *song_check = CheckTheFile(file);
        if(!song_check){
            return 1;
        }
        int correct = 0;
        PlayingSong(song_check, &correct);
        if(correct == 1){
            fprintf(stderr,"Playing went wrong\n");
        }
        fclose(song_check);
    }
    char input;
    printf("Do you want to create a playlist y/n \n");
    scanf(" %c", &input);
    while ((q1 = getchar()) != '\n' && q1 != EOF) { }
    if (input == 'y' || input == 'Y') {
        bool con = true;
       int rc;
       sqlite3 *db = OpenDB(&rc);
        while(con){
            char song[256];
            printf("past the file adress of the song \n");
            if(fgets(song, sizeof(song),stdin) != NULL){
                size_t len  =strlen(song);
                if(len > 0 && song[len-1] == '\n'){
                    song[len-1] = '\0';
                }

            }else {
            fprintf(stdout,"wrong path");
                continue;
            }
            while ((q1 = getchar()) != '\n' && q1 != EOF) { }
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
            char q3;
            printf("do you want to add more songs? y/n");
            scanf("%c",&q3);
            if(q3 == 'n' || q3 == 'N'){
                con = false;
            }
        }
        sqlite3_close(db);
    }else if (input == 'n') {
        return 0;
    }else {
        printf("wrong input \n");
    }
return 0;
}


