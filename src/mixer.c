#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "headers/read_platlist.h"
volatile bool play_flag = true;
volatile bool skip_request = false;
volatile int direction = 0;
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
}
void user_input(){
    char c[256];
    char n;
    set_nonblock(1);
    while(play_flag){
        read(STDIN_FILENO, c, 1);
        if(strcmp(c,"skip") == 0){

        }
        if(strcmp(c,"previous") == 0){

        }
        if(strcmp(c,"stop") == 0){
        }
        if(!play_flag){
            printf("No more song in playlist \n");
            break;
        }



    }

}
//void *input_thread(void *arg){




