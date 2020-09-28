// Take an array of words and number i. Delete i-th and (i+1)-th words from 
// the array.
void remove_two_words(char **list, int i);

// Search for ">" and "<" words. If possible, redirect output or input 
// respectively by calling redirect_in and redirect_out. If no
// redirection symbols found, assign -1 to the given file descriptors. 
void redirect(char **list, int *inputfd, int *outputfd);

// Open file to read/write, and assign its file descriptor to
// *outputfd. If there's no such file, create it.
void redirect_out(char *filename, int *outputfd);

// Open file to read, and assign its file descriptor to *inputfd.
void redirect_in(char *filename, int *inputfd);

// Restore stdin and stdout, and close files with the given file descriptors
// if they are opened.
void restore_io(int inputfd, int outputfd,
                const int constSTDIN, const int constSTDOUT);
