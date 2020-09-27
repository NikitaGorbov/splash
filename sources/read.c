#include <stdio.h>
#include <stdlib.h>

char *get_word(int *endOfLineFlag) {
    char *word = NULL;
    char c;
    int i = 0;

    do {
        c = getchar();
        word = realloc(word, sizeof(char) * i + 2);
        word[i] = c;
        i++;
    } while (c != ' ' && c != '\n');
    // Set end of line flag if the last character was '\n'
    *endOfLineFlag = (word[i - 1] == '\n');
    // Null terminate the word
    word[i - 1] = 0;
    return word;
}

char **get_list() {
    char **list = NULL;
    int i = 0, flag = 0;

    do {
        list = realloc(list, sizeof(char *) * i + 1);
        list[i] = get_word(&flag);
        i++;
    } while (!flag);
    // Add NULL pointer at the end of the command for further execution
    list[i] = NULL;
    return list;
}

