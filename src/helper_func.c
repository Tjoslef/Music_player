#include "headers/helpers.h"
void clear_input_buffer() {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
}
 char get_user_input(const char *prompt) {
        char input;
        printf("%s", prompt);
        scanf(" %c", &input);
        clear_input_buffer();
        return input;
}

