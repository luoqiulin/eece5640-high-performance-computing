#include <stdio.h>

int main(void) {
  
  // 2d array
  int num[3][4] = {
    {1, 2,  3,  4},
    {5, 6,  7,  8},
    {9, 10, 11, 12}
  };

  // pointer ptr pointing at array num
  int *ptr = &num[0][0];
  
  // other variables
  int
    ROWS = 3,
    COLS = 4,
    TOTAL_CELLS = ROWS * COLS,
    i;
  
  // print the elements of the array num via pointer ptr
  for (i = 0; i < TOTAL_CELLS; i++) {
    printf("%d ", *(ptr + i));
  }
  
  return 0;
}

