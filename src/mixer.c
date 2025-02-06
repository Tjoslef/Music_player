#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include "headers/read_platlist.h"
volatile bool play_flag = true;
volatile bool skip_request = false;
volatile int direction = 0;
SongNode *current = NULL;

//void *input_thread(void *arg){}
