#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/read.h"
#include "../include/redirect.h"

// Free array of commands
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
    int inputfd, outputfd;
    // Save stdin and stdout file descriptors
    const int constSTDIN = dup(0);
    const int constSTDOUT = dup(1);
    while (1) {
        cmd = NULL;
        printf("$ ");
        // Get a command
        cmd = get_list();
        // Redirect input and output if needed
        redirect(cmd, &inputfd, &outputfd);
        // Check if 'quit' or 'exit' command was given
        if (cmd[0] && (!strcmp(cmd[0], "exit") || !strcmp(cmd[0], "quit"))) {
            break;
        }
        // Execute the given command in a child process
        if (fork() > 0) {
            wait(NULL); 
        } else {
            if (execvp(cmd[0], cmd) < 0) {
                write(constSTDOUT, "Unknown command\n", 16);
                restore_io(inputfd, outputfd, constSTDIN, constSTDOUT);
                free_cmd(cmd);
                exit(0);
            }
        }
        // Free array of commands and restore stdin/stdout for further command
        // execution
        free_cmd(cmd);
        restore_io(inputfd, outputfd, constSTDIN, constSTDOUT);
    }
    free_cmd(cmd);
    return 0;
}
