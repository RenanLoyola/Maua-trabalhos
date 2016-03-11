#include <stdio.h>

void swap(int *x, int *y){
    int temp;
    temp=*x;
    *x=*y;
    *y=temp;
}

int main(void) {
    int x=10, y=20 ;
    swap(&x,&y);
    printf("x= %d, y= %d \n", a, b);
    return 0;
}