#include "head.h"
int main(int argc,char **argv){
    if(argc<2){printf("Usage: -m a b | -q eps a b c | -t eps a b c\n");return 1;}
    if(argv[1][0]!='-'||argv[1][2]!='\0'){printf("Wrong flag\n");return 1;}
    char flag=argv[1][1]; int need=flag=='m'?2:4;
    if((flag!='m'&&flag!='q'&&flag!='t')||argc!=need+2){printf("Wrong flag or argument count\n");return 1;}
    double v[4]; for(int i=0;i<need;i++)v[i]=atof(argv[i+2]);
    if(flag=='m'){int a=(int)v[0],b=(int)v[1];if(v[0]!=a||v[1]!=b||a==0||b==0){printf("Need nonzero integers\n");return 1;}printf("%s\n",a%b==0?"multiple":"not multiple");}
    else if(v[0]<=0){printf("epsilon must be positive\n");return 1;}
    else if(flag=='t')printf("%s\n",triangle(v[1],v[2],v[3],v[0])?"right triangle possible":"not a right triangle");
    else {int p[6][3]={{0,1,2},{0,2,1},{1,0,2},{1,2,0},{2,0,1},{2,1,0}};for(int i=0;i<6;i++)solve(v[1+p[i][0]],v[1+p[i][1]],v[1+p[i][2]],v[0]);}
    return 0;
}
