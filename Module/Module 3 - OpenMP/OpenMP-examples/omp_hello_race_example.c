#include <stdio.h>
#include <unistd.h>

int main(void)
{
#pragma omp parallel
   {    
    printf("Hello\n");
    sleep(1);
    printf("world.\n");
   }
   return 0;
}
