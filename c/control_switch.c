#include <stdio.h>

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
* argc - The number of command line arguments
* argv - A pointer to the first argument (text string)
*/
int main (int argc, char* argv[]) {

  switch (argc) {
    // In case of no extra arguments
    case 1:
      printf("Hello world!\n");
      break;

    // In case of one argument
    case 2:
      printf("Hello %s!\n", argv[1]);
      break;

    // Else
    default: 
      printf("Wrong usage\n");
      break;
  }

  return 0;

}
