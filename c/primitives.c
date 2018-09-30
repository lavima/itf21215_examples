#include <stdio.h>
#include <stdlib.h>

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
* argc - The number of command line arguments
* argv - A pointer to the first argument (text string)
*/
int main (int argc, const char* argv[]) {

  // Make sure the number of arguments are correct
  if (argc != 4) {
    printf("Wrong usage\n");
    return 0;
  }

  // Store the input arguments in appropriate primitives
  const char *name = argv[1];
  int age = atoi(argv[2]);
  float height = atof(argv[3]);

  printf("Name: %s Age: %d Height: %f\n", name, age, height);

  return 0;

}
