#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define SIZE 10000


int main(){
    FILE* ptr;
    ptr = fopen("input.txt", "w");

     if (ptr == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    fprintf(ptr, "%d %d\n", SIZE, SIZE);
     srand((unsigned)time(NULL));
   for (size_t i = 0; i < SIZE; i++)
   {
    int a = (rand()% SIZE) + 1;
    int b = (rand()% SIZE) + 1;
    fprintf(ptr, "%d %d\n", a, b);
   }

   fclose(ptr);
    printf("Input file 'input.txt' has been generated.\n");

    return 0;
}