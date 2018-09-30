#include <stdio.h>
#include <stdlib.h>

// A global counter
int globalCounter = 0;

/*
* Function for adding to the global counter.
*/
void add(int a) { globalCounter += a; }

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
*/
int main (int argc, char* argv[]) {

  int num = atoi(argv[1]);
  for (int i=0; i<num; ++i)
    add(i);

  printf("global=%d\n", globalCounter);

  return 0;

}
