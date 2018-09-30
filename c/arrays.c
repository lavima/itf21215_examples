#include <stdio.h>

/*
* Function for calculating the sum of an integer array
*/
int sum(int array[], int num) {

  int sum=0;

  for (int i=0; i<num; ++i)
    sum += array[i];

  return sum;

}

/*
* This the program entry function. The parameters provide a means to access the
* command line arguments.
*/
int main (int argc, char* argv[]) {

  int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

  printf("sum=%d\n", sum(numbers, 9));

  return 0;

}
