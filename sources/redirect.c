#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../include/redirect.h"

void remove_two_words(char **list, int i) {
    free(list[i]);
    free(list[i + 1]);
    while (list[i + 2]) {
        list[i] = list[i + 2];
        i++;
    }
    list[i] = NULL;
    list[i + 1] = NULL;
}

void redirect_out(char *filename, int *outputfd) {
    *outputfd = open(filename, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    dup2(*outputfd, 1);
}

void redirect_in(char *filename, int *inputfd) {
    *inputfd = open(filename, O_RDONLY);
    dup2(*inputfd, 0);
}

void redirect(char **list, int *inputfd, int *outputfd) {
    int i = 0;

    *inputfd = -1;
    *outputfd = -1;
    while (list[i]) {
        if (!strcmp(list[i], ">")) {
            if (list[i + 1]) {
                redirect_out(list[i + 1], outputfd);
                remove_two_words(list, i);
                // i-th word changed, so it needs to be checked again.
                i--;
            } else {
                printf("Output file not specified\n");
            }
        }
        if (!strcmp(list[i], "<")) {
            if (list[i + 1]) {
                redirect_in(list[i + 1], inputfd);
                remove_two_words(list, i);
                // i-th word changed, so it needs to be checked again.
                i--;
            } else {
                printf("Input file not specified\n");
            }
        }
        i++;
    }
}

void restore_io(int inputfd, int outputfd,
                const int constSTDIN, const int constSTDOUT) {
    if (outputfd != -1) {
        close(outputfd);
        dup2(constSTDOUT, 1);
    }
    if (inputfd != 1) {
        close(inputfd);
        dup2(constSTDIN, 0);
    }
}
