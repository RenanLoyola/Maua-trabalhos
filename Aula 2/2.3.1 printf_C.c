#include <stdio.h>

const int sizee = 20;

int main()
{

   int arr[]={2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71};
   int i;
   
  for (i = 0 ; i<sizee;i++){
    printf ("Primo - %d ",i+1);
    printf ("= %d \n", arr[i]);
  }
  
  return 0;
}