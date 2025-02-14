#include "./headers/playing.h"
#include "./headers/playlist.h"
#include "./headers/read_platlist.h"
#include "./headers/audio.h"
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
volatile bool skip_flag = false;
pthread_mutex_t skip_mutex = PTHREAD_MUTEX_INITIALIZER;
char *trimming(char *name){
    if(name == NULL) return NULL;
    while(isspace((unsigned char) *name)) name++;
    if(*name == 0) return strdup("");
    char *trimmed = malloc(strlen(name) + 1);
    if (trimmed == NULL) return NULL;
    strcpy(trimmed, name);
    char *end = trimmed + strlen(trimmed) - 1;
    while(end > trimmed && isspace((unsigned char) *end)) end --;
    *(end+ 1) = '\0';
    return trimmed;
}
char get_user_input(const char *prompt) {
        char input[256];
        printf("%s "  , prompt);
        if(fgets(input, sizeof(input),stdin) != NULL){
            input[strcspn(input,"\n")] = 0;
            return input[0];

    }
    return '\0';
}
void set_nonblock(int state) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);
    if (state == 1) {
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_cc[VMIN] = 1;
    } else if (state == 0) {
        ttystate.c_lflag |= ICANON;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}int main(){
    int rc;
    sqlite3 *db = NULL;
    bool cont = true;
while(cont){
    char *q1 = "Options:\n"
                   "1. Play a song\n"
                   "2. Create playlist\n"
                   "3. Play from playlist\n"
                   "4. Help\n"
                   "Enter your choice: ";
    char an1 = get_user_input(q1);
    switch (an1) {
        case '1':
            {
            char file[256];
            printf("Give path to file \n :");
            if (fgets(file, sizeof(file), stdin) != NULL) {
               file[strcspn(file, "\n")] = '\0';
                if (CheckTheFile(file) != 1) {
                    int correct = 0;
                    PlayingSong(file, &correct);
                    if (correct != 0) {
                        fprintf(stderr, "Playing went wrong\n");
                    }
                }
            }
            }
            break;
        case '2':
            {
            db = OpenDB(&rc);
            if (db == NULL) {
                fprintf(stderr, "Failed to open database\n");
                 return 1;
            }
            char dbName[256];
            int con = true;
            printf("What name do you want for it \n");
            if(fgets(dbName, sizeof(dbName),stdin) != NULL){
                int id_playlist = -1;
                createPlaylist(db,trimming(dbName),&id_playlist);
                while(con){
                    char song[256];
                    printf("past the file adress of the song \n");
                    if(fgets(song, sizeof(song),stdin) != NULL){
                         song[strcspn(song, "\n")] = '\0';
                    }else {
                    fprintf(stdout,"file has to have path to it");
                        continue;
                    }
                    if(CheckTheFile(song) == 1){
                        printf("wrong path");
                        continue;
                    }
                const char *name = getName(song);
                printf("name of song: %s \n",name);
               if(insertPlaylist(song,db,name,id_playlist) != '\0'){
                    fprintf(stderr, "Failed to insert song into playlist\n");
                }
                char *q3 = "do you want to add more songs? (y/n) \n";
                char an3 = get_user_input(q3);
                if(an3 == 'n' || an3 == 'N'){
                    con = false;
                }
            }
            sqlite3_close(db);
        }
            }
            break;
        case '3':
            {
                db = OpenDB(&rc);
                if (db == NULL) {
                    fprintf(stderr, "Failed to open database \n");
                    return 1;
                }
                printf("what is name of playlist \n");
                char an5[256];
                if(fgets(an5,sizeof(an5) ,stdin) != NULL){

                    int id = finding_playlist(db,trimming(an5));
                    if(id != -1){
                        SongNode *head = reading_in_order(db,id);
                        if(head == NULL){
                            fprintf(stderr,"something of in reading playlist \n");
                        }
                            while(head != NULL){
                                set_nonblock(1);
                                char c[256];
                                char n;
                                printf("playing.. \n");
                                int correct = 0;
                                ThreadArgs args = {
                                    .path = head->path,
                                    .correct = &correct,
                                    .skip_flag = &skip_flag

                                };
                                pthread_t tread_id;
                                if(pthread_create(&tread_id,NULL,PlayingSong,&args) != 0){
                                continue;
                            }
                            while(1){
                            if(read(STDIN_FILENO,c,sizeof(c)) >0){
                                if(strcmp(c,"skip") == 0){
                                    pthread_mutex_lock(&skip_mutex);
                                    skip_flag = true;
                                    pthread_mutex_unlock(&skip_mutex);
                                    break;

                                }
                            }
                            usleep(10000);
                            }
                            pthread_join(tread_id,NULL);
                            if (correct != 0) {
                               fprintf(stderr, "Failed playing song: %s\n", head->name);
                            }
                            pthread_mutex_lock(&skip_mutex);
                            skip_flag = false;
                            pthread_mutex_unlock(&skip_mutex);
                            head = head->next;
                        }
                }
            }
            }
                break;
        case '4':
            {
            printf("1.option you put path of song you want to play \n");
            printf("2.option you can create your playlist \n");
            printf("3.option you can play playlist which you already created \n");
            }
            break;
        default:
            printf("wrong input");
    }
        int c;
       while ((c= getchar()) != '\n' && c != EOF){}
        char *q2 = "Do you want to continue (y/n) ?";
        char an = get_user_input(q2);
        if(an != 'y' && an != 'Y'){
            cont = false;
        }
    }
return 0;
}


