#include "head.h"
int main(int argc,char **argv){
    if(argc!=2){printf("Usage: ./program epsilon\n");return 1;}
    double eps=atof(argv[1]);
    if(eps<=0||eps>=1){printf("epsilon must be between 0 and 1\n");return 1;}
    const char *names[]={"e","pi","ln2","sqrt2","gamma"};
    for(int i=0;i<5;i++){double a,b,c;int ok=compute(i,eps,&a,&b,&c);printf("%s: series=%.12g equation=%.12g limit=%.12g%s\n",names[i],a,b,c,ok?"":" (iteration limit reached)");}
    return 0;
}
