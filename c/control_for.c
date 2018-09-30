#include <stdio.h>

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
* argc - The number of command line arguments
* argv - A pointer to the first argument (text string)
*/
int main (int argc, const char* argv[]) {

  // Run a loop through all the command line arguments
  for (int i=1; i<argc; ++i)
    printf("Hello %s!\n", argv[i]);

  return 0;

}
