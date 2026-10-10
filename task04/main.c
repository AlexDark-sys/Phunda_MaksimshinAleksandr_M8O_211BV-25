#include "head.h"
int main(int argc,char **argv){
    if(argc<3||argc>4){printf("Usage: flag input [output]\n");return 1;}
    int named=argv[1][1]=='n';char op=named?argv[1][2]:argv[1][1];
    if(argv[1][0]!='-'||!strchr("disa",op)){printf("Bad flag\n");return 1;}
    if((named&&argc!=4)||(!named&&argc!=3)){printf("Check arguments\n");return 1;}
    char out[512]; if(named)snprintf(out,sizeof(out),"%s",argv[3]);else snprintf(out,sizeof(out),"out_%s",argv[2]);
    return transform(argv[2],out,op);
}
