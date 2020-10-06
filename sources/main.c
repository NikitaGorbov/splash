#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "read.h"
#include "redirect.h"

// Check if cmd contains pipeline instructions ("|"). If so, remove "|",
// assign the number of pipes to the given variable and return an array of
// pointers to each command in the pipeline
char ***make_pipeline(char **cmd, int *numberOfPipes) {
    int i;
    *numberOfPipes = 0;
    char ***pipe = malloc(sizeof(char **) * 2);
    pipe[0] = &cmd[0];
    for (i = 0; cmd[i]; i++) {
        if (!strcmp(cmd[i], "|")) {
            if (!cmd[i + 1]) {
                break;
            }
            free(cmd[i]);
            cmd[i] = NULL;
            pipe = realloc(pipe, sizeof(char **) * (i + 2));
            (*numberOfPipes)++;
            pipe[*numberOfPipes] = &cmd[i + 1];
        }
    }
    // Set the last pointer to NULL
    pipe[*numberOfPipes + 1] = NULL;
    return pipe;
}

// Free the array of commands
void free_cmd(char **cmd) {
    int i = 0;
    while (cmd[i]) {
        free(cmd[i]);
        i++;
    }
}

int main() {
    char **cmd, ***pipel = NULL;
    int inputfd, outputfd, numberOfPipes, pid, i;
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
        // Check if the given command needs pipeline
        pipel = make_pipeline(cmd, &numberOfPipes);
        // Execute the given command in a child process if no pipeline needed
        if (numberOfPipes == 0) {
            if (fork() == 0) {
                if (execvp(cmd[0], cmd) < 0) {
                    write(constSTDOUT, "Unknown command\n", 16);
                    free_cmd(cmd);
                    free(cmd);
                    exit(1);
                }
            } else {
                wait(NULL);
            }
            // Execute pipeline
        } else {
            // Create an array of pipes
            int (*fd)[2] = malloc(sizeof(int) * 2 * (numberOfPipes + 1));
            for (i = 0; i <= numberOfPipes; i++) {
                pipe(fd[i]);
            }
            // Make the first command in the pipeline read from the given input
            // file
            dup2(0, fd[0][0]);
            for (i = 0; i <= numberOfPipes; i++) {
            redirect(pipel[i], &inputfd, &outputfd);
                if ((pid = fork()) == 0) {
                    dup2(fd[i][0], 0);
                    close(fd[i][0]);
                    close(fd[i][1]);
                if (i != numberOfPipes) {
                    dup2(fd[i + 1][1], 1);
                    close(fd[i + 1][0]);
                    close(fd[i + 1][1]);
                }
                   if (execvp(pipel[i][0], pipel[i]) < 0) {
                       write(constSTDOUT, "Unknown command\n", 16);
                       exit(1);
                   }
                   // Parent process
                } else {
                    close(fd[i][0]);
                    close(fd[i][1]);
                    waitpid(pid, NULL, 0);
                }
            }
            free(fd);
        }
        // Free pipeline, array of commands, and restore stdin/stdout for
        // further command execution
        i = 0;
        while (pipel[i]) {
            free_cmd(pipel[i]);
            i++;
        }
        free(pipel);
        free(cmd);
        restore_io(inputfd, outputfd, constSTDIN, constSTDOUT);
    }
    // Free array of commands
    free_cmd(cmd);
    free(cmd);
    return 0;
}
