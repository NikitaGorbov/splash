#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <signal.h>
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

// Searh the given command for the word "cd". If there is such word, change
// directory, set the environment variable PWD accordingly and return 1. If
// directory wasn't changed return 0;

int change_directory(char **command) {
    char *PWD = getenv("PWD");
    char *path = NULL;

    if (!strcmp(command[0], "cd")) {
        if (!command[1] || !strcmp(command[1], "~")) {
            // Change directory to user's home
            path = malloc(sizeof(char) * strlen(getenv("HOME")) + 1);
            strcpy(path, getenv("HOME"));
        } else {
            path = malloc(sizeof(char) * strlen(command[1]) + 1);
            strcpy(path, command[1]);
        }
        // Change current working directory
        if (chdir(path)) {
            printf("Unable to change directory\n");
            free(path);
            return 0;
        }
        // Use relative path
        // if (path[0] != '/') {
        //     strcat(PWD, "/");
        //     strcat(PWD, path);
        // } else {
        //     // Use absolute path
        //     strcpy(PWD, path);
        // }
        PWD = getcwd(PWD, strlen(PWD) + strlen(path) + 1);

        free(path);
        return 1;
    }
    return 0;
}

// Check if command contains the word "&" at the end. If so, delete it and
// return 1. Otherwise return 0;
int background(char **command) {
    int i;
    for (i = 0; command[i]; i++) {
        if (!strcmp(command[i], "&")) {
            if (!command[i + 1]) {
                free(command[i]);
                command[i] = NULL;
                return 1;
            }
        }
    }
    return 0;
}

void execute(char **command, const int constSTDOUT) {
    if (execvp(command[0], command) < 0) {
        write(constSTDOUT, "Unknown command\n", 16);
        exit(1);
    }
}

int main() {
    char **cmd, ***pipel = NULL;
    int inputfd, outputfd, numberOfPipes, pid, i, bgFlag, *bgPids = NULL,
        bgCounter;
    // Save stdin and stdout file descriptors
    const int constSTDIN = dup(0);
    const int constSTDOUT = dup(1);

    // Terminate currently working program when recieve SIGINT. Defined in main
    // to have an access to bgPids array
    void sig_handler(int sig) {
        kill(pid, 2);
        write(constSTDOUT, "\n", 1);
    }

    signal(SIGINT, sig_handler);

    bgCounter = 0;

    while (1) {
        cmd = NULL;
        printf("$ ");
        // Get a command
        cmd = get_list();
        // Check if 'quit' or 'exit' command was given
        if (cmd[0] && (!strcmp(cmd[0], "exit") || !strcmp(cmd[0], "quit"))) {
            break;
        }
        // Redirect input and output if needed
        redirect(cmd, &inputfd, &outputfd);
        // Check if the given command needs pipeline
        pipel = make_pipeline(cmd, &numberOfPipes);
        // Execute the given command in a child process if no pipeline needed
        // Create an array of pipes
        int (*fd)[2] = malloc(sizeof(int) * 2 * (numberOfPipes + 1));
        for (i = 0; i <= numberOfPipes; i++) {
            pipe(fd[i]);
        }
        // Make the first command in the pipeline read from the given input
        // file
        dup2(0, fd[0][0]);
        for (i = 0; i <= numberOfPipes; i++) {
            bgFlag = background(pipel[i]);
            if (change_directory(pipel[i])) {
                // cd cannot procuce any output (except for an error message) or
                // recieve any input
                break;
            }
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
                execute(pipel[i], constSTDOUT);
                // Parent process
            } else {
                close(fd[i][0]);
                close(fd[i][1]);
                if (!bgFlag) {
                    waitpid(pid, NULL, 0);
                } else {
                    bgPids = realloc(bgPids, sizeof(int) * (bgCounter + 1));
                    bgPids[bgCounter] = pid;
                    bgCounter++;
                }
            }
        }
        free(fd);
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
    // Free array of commands and finish background processes
    free_cmd(cmd);
    free(cmd);
    if (bgCounter) {
        while (bgCounter) {
            kill(bgPids[bgCounter - 1], 2);
            bgCounter--;
        }
        free(bgPids);
    }
    return 0;
}
