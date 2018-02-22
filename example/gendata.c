#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define N 1000000
int main(int argc,char *argv[]) {
    unsigned char **a;
    int n,i,j;
    FILE *fp;
    n=atoi(argv[1]);
    a=(unsigned char **)calloc(n,sizeof(unsigned char *));
    for(i=0;i<n;i++) {
        a[i]=(unsigned char *)malloc(16);
        if(a[i]==NULL) {
            printf("Failed to allocate memory\n");
            exit(1);
        }
    }

    if((fp=fopen(argv[2],"w"))==NULL) {
        fprintf(stderr,"Failed to open file FPGA.data\n");
        return EXIT_FAILURE;
    }

    srand((int)time(NULL)); 
    for(i=0;i<n;i++) {
        for(j=0;j<8;j++) {
            a[i][15-j]=rand()%256;
        }
        for(j=8;j<16;j++) {
            if(j<12) {
                a[i][15-j]=rand()%256;
            }
            else {
                a[i][15-j]=0x00;
            }
        }
        fwrite(a[i],sizeof(unsigned char),16,fp);
    }
    fclose(fp);
    for(i=0;i<n;i++) {
        free(a[i]);
    }
}
