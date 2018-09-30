#include <stdio.h>
#include <stdlib.h>

// A structure for holding the person information
typedef struct {
  const char *name;
  int age;
  float height;
} person;

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
  person *p = malloc(sizeof(person));
  p->name = argv[1];
  p->age = atoi(argv[2]);
  p->height = atof(argv[3]);

  printf("Name: %s Age: %d Height: %f\n", p->name, p->age, p->height);

  return 0;

}
