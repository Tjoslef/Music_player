#include <stdio.h>
#include <string.h>
#include "headers/read_platlist.h"
volatile bool play_flag = true;
volatile bool skip_request = false;
volatile int direction = 0;
/*
    * void user_input(){
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

}o
    */
//void *input_thread(void *arg){




