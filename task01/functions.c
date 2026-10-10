#include "head.h"
int is_prime(int n) { if(n<2)return 0; for(int i=2;i<=n/i;i++) if(n%i==0)return 0; return 1; }
void print_hex_digits(unsigned int n) { char b[16]; int i=0; if(n==0){printf("0\n");return;} while(n>0){b[i++]="0123456789ABCDEF"[n%16];n/=16;} while(i>0)printf("%c",b[--i]); printf("\n"); }
int factorial(int n,unsigned int *result) { *result=1; for(int i=2;i<=n;i++){if(*result>2147483647u/(unsigned int)i)return 0;*result*=(unsigned int)i;}return 1; }
