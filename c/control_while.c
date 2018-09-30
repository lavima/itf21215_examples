#include <stdio.h>

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
* argc - The number of command line arguments
* argv - A pointer to the first argument (text string)
*/
int main (int argc, const char* argv[]) {

  // Run a loop through all the command line arguments
  int i=1;
  while (i<argc) {
    printf("Hello %s!\n", argv[i]);
    ++i;
  }

  return 0;

}
