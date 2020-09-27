#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/read.h"

// Free given array of words
void free_cmd(char **cmd) {
    int i = 0;
    while(cmd[i]) {
        free(cmd[i]);
        i++;
    }
    free(cmd);
}

int main() {
    char **cmd;
    while (1) {
        cmd = NULL;
        printf("$ ");
        // Get a command
        cmd = get_list();
        // Check if 'quit' or 'exit' command was given
        if (cmd[0] && (!strcmp(cmd[0], "exit") || !strcmp(cmd[0], "quit"))) {
            break;
        }
        // Execute the given command in a child process
        if (fork() > 0) {
            wait(NULL); 
        } else {
            if (execvp(cmd[0], cmd) < 0) {
                printf("Unknown command\n");
                free_cmd(cmd);
                exit(0);
            }
        }
        free_cmd(cmd);
    }
    free_cmd(cmd);
    return 0;
}
