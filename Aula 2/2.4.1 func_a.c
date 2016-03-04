#include <stdio.h>

int MDC(int num1, int num2){
int i;
 int resultado;
 for ( i =1; i<=num1 || i<=num2 ; ++i ){
    if (num1%i==0 && num2%i==0)
     resultado=i ;
 }
 
return(resultado);
}

int main() {
int a = 12 , b =64, i , MD ;

/*
printf ( "Numero 1 : \n" ) ;
scanf ( "%d" , &a ) ;
printf ( "Numero 2 : \n" ) ;
scanf ( "%d" , &b ) ;
*/

MD=MDC(a,b);

printf("resultado= %d\n", MD);

}
