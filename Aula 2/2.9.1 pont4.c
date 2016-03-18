 2.9.1 pont4.c:



@@ -0,0 +1,18 @@
#include <stdio.h>

int print_array(int n, int *test_array);
{
    int i;
    for (i=0, i<n, i++);
    printf *("valor =%d\n", *(arr+i));
}

int main()
{
    int test_array[] = {1,2,3,4,5};
    int n = 5;
    print_array(n, test_array);

    return 0;
}