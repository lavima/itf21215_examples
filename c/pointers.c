#include <stdio.h>

/*
* Function for adding two numbers together. The result is stored in a.
*/
void add(int *a, int b) {
  *a = *a + b;
}

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
*/
int main (int argc, char* argv[]) {

  int n1 = 11;
  int n2 = 1;

  add(&n1, n2);

  printf("n1=%d n2=%d\n", n1, n2);

  return 0;

}
