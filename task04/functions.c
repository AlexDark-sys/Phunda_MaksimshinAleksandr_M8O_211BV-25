#include "head.h"
int transform(const char *in,const char *out,char op){
    FILE *a=fopen(in,"r"); if(!a){printf("Cannot open input file: %s\n",in);return 1;}
    FILE *b=fopen(out,"w"); if(!b){printf("Cannot open output file: %s\n",out);fclose(a);return 1;}
    char line[4096];
    while(fgets(line,sizeof(line),a)){
        int n=(int)strlen(line);
        if(op=='d'){for(int i=0;i<n;i++)if(line[i]<'0'||line[i]>'9')fputc(line[i],b);}
        else if(op=='i'||op=='s'){int count=0;for(int i=0;i<n;i++){char c=line[i];int letter=(c>='A'&&c<='Z')||(c>='a'&&c<='z');int digit=c>='0'&&c<='9';if((op=='i'&&letter)||(op=='s'&&!letter&&!digit&&c!=' '&&c!='\n'&&c!='\t'&&c!='\r'))count++;}fprintf(b,"%d\n",count);}
        else {for(int i=0;i<n;i++){unsigned char c=(unsigned char)line[i];if((c>='0'&&c<='9')||c=='\n'||c=='\r')fputc(c,b);else fprintf(b,"%X",c);}}
    }
    fclose(a);fclose(b);return 0;
}
