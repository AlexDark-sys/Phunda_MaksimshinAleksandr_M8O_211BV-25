#include "head.h"
int main(int argc, char **argv) {
    int x;
    if (argc != 3) { printf("Usage: ./program number -h|-p|-s|-e|-a|-f\n"); return 1; }
    x = atoi(argv[1]);
    if (x < 0) { printf("Number must be nonnegative\n"); return 1; }
    if (argv[2][0] != '-' || argv[2][2] != '\0') { printf("Unknown flag\n"); return 1; }
    switch (argv[2][1]) {
        case 'h':
            if (x == 0) { printf("No positive multiples of 0\n"); break; }
            { int found = 0; for (int i=1; i<=100; i++) if (i%x==0) { printf("%d ",i); found=1; }
              if (!found) printf("No multiples in [1,100]");
              printf("\n"); }
            break;
        case 'p': printf("%d is %s\n",x,is_prime(x)?"prime":(x>1?"composite":"neither prime nor composite")); break;
        case 's': print_hex_digits((unsigned int)x); break;
        case 'e':
            if (x>10) { printf("For -e number must be <= 10\n"); return 1; }
            for (int i=1;i<=10;i++) for (int j=1;j<=x;j++) { int v=1; for(int k=0;k<j;k++){ if(v>2147483647/i){printf("Result is too large for int\n");return 1;} v*=i; } printf("%d^%d=%d\n",i,j,v); }
            break;
        case 'a': if(x>65535){printf("Sum is too large for int\n");return 1;} printf("%d\n",x*(x+1)/2); break;
        case 'f': { unsigned int r=1; if (!factorial(x,&r)) printf("Factorial is too large\n"); else printf("%u\n",r); } break;
        default: printf("Unknown flag\n"); return 1;
    }
    return 0;
}
