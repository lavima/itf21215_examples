#include <stdio.h>

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
* argc - The number of command line arguments
* argv - A pointer to the first argument (text string)
*/
int main (int argc, char* argv[]) {

  // Print error and return if the number of arguments is larger 
  // than 1 (only the program)
  //if (argc > 1) {
  if (10) {
    printf("Wrong usage\n");
    return 0; 
  }

  // Else
  printf("Hello world!\n");
  return 0;

}
