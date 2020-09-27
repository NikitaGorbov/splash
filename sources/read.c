#include <stdio.h>
#include <stdlib.h>

char *get_word(int *endOfLineFlag) {
    char *word = NULL;
    char c;
    int i = 1;

    do {
        c = getchar();
        word = realloc(word, sizeof(char) * (i + 1));
        word[i - 1] = c;
        i++;
    } while (c != ' ' && c != '\n');
    // Set end of line flag if the last character was '\n'
    *endOfLineFlag = (word[i - 2] == '\n');
    // Null terminate the word
    word[i - 2] = 0;
    return word;
}

char **get_list() {
    char **list = NULL;
    int i = 1, flag = 0;

    do {
        list = realloc(list, sizeof(char *) * (i + 1));
        list[i - 1] = get_word(&flag);
        i++;
    } while (!flag);
    // Add NULL pointer at the end of the command for further execution
    list[i - 1] = NULL;
    return list;
}

