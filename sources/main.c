#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/read.h"

int main() {
    char **cmd = NULL;
    while (1) {
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
                exit(0);
            }
        }
    }
    return 0;
}
