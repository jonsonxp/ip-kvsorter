#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#define K 1024 //max_k_number
#define FIFO_SIZE 16*100
#define WRITE_BUFF_SIZE 16*100  //Bytes 16B=128b
#define READ_BUFF_SIZE 16*1000
#define DATAWIDTH 16  //Bytes
typedef struct {
    unsigned char flag_end;
    unsigned char flag_channel;
    unsigned char reserved_part1;
    unsigned char reserved_part2;
    unsigned char data_part4;
    unsigned char data_part3;
    unsigned char data_part2;
    unsigned char data_part1;
    unsigned char key_part8;
    unsigned char key_part7;
    unsigned char key_part6;
    unsigned char key_part5;
    unsigned char key_part4;
    unsigned char key_part3;
    unsigned char key_part2;
    unsigned char key_part1;
} myDataType;

void f_quick_sort(myDataType *data, int left, int right);
unsigned int f_get_data(myDataType *item);
void f_swap(myDataType *item_A, myDataType *item_B);
double f_get_time();
int data_cnt;
int main(int argc, char *argv[]) {
    myDataType *data;
    myDataType *data_tmp;
    FILE *fp_read;
    FILE *fp_write;
    int k; //k-way merge tree
    int item_total_number,item_HW,item_SW;
    double time_read_start,time_read_end,time_write_start,time_write_end;
    double *time_start_loop,*time_end_loop;
    int index;
    long long int item_tmp;

    data_cnt = atoi(argv[1]);
    data = (myDataType *)calloc(data_cnt, sizeof(myDataType));
//-------------------------------------------------
// Open data file
//-------------------------------------------------    
    fp_read=fopen(argv[2],"r");
    if(fp_read==NULL) {
        printf("%s open failed: %s\n",argv[2],strerror(errno));
        return EXIT_FAILURE;
    }
//-------------------------------------------------    
// Read data to memory
//-------------------------------------------------    
    time_read_start=f_get_time();
    item_total_number=fread(data,sizeof(myDataType), data_cnt, fp_read);
    printf("There are %d items in this file\n",item_total_number);
    time_read_end=f_get_time();
    printf("Read time is %.6f s\n",time_read_end-time_read_start);

//-------------------------------------------------    
//QuickSort
//-------------------------------------------------    

    time_read_start=f_get_time();
    f_quick_sort(data,0,item_total_number-1);
    time_read_end=f_get_time();
    printf("Quick sort time is %.6f s\n",time_read_end-time_read_start);

//    for(index=0;index<item_total_number;index++) {
//        item_tmp=f_get_data(&data[index]);
//        printf("Item %d is %lli\n",index,item_tmp);
//    }

//------------------------------------------------------    
// Write data to file    
//------------------------------------------------------    
    fp_write=fopen(argv[3],"w");
    if(fp_write==NULL) {
        printf("%s open failed: %s\n",argv[3],strerror(errno));
        return EXIT_FAILURE;
    }
    
    time_write_start=f_get_time();
    fwrite(data,sizeof(myDataType), data_cnt, fp_write);
    time_write_end=f_get_time();
    printf("Write time is %.6f s\n",time_write_end-time_write_start);
    
    free(data);
    fclose(fp_read);
    fclose(fp_write);

    return 0;
}

void f_quick_sort(myDataType *data,int left,int right) {
    int i=left,j=right;
    int pivot;
    pivot=f_get_data(&data[(left+right)/2]);
    while(1) {
        while(f_get_data(&data[i])<pivot) {
            i++;
        }
        while(f_get_data(&data[j])>pivot) {
            j--;
        }
        if(i>=j)
            break;
        f_swap(&data[i],&data[j]);
        i++;
        j--;

    }
    if(left<i-1)
        f_quick_sort(data,left,i-1);
    if(j+1<right)
        f_quick_sort(data,j+1,right);
}
unsigned int f_get_data(myDataType *item) {
    unsigned int item_value=0;
    item_value|=(unsigned int)(*item).data_part1<<8*3;
    item_value|=(unsigned int)(*item).data_part2<<8*2;
    item_value|=(unsigned int)(*item).data_part3<<8*1;
    item_value|=(unsigned int)(*item).data_part4<<8*0;
    return item_value;
}
void f_swap(myDataType *item_A,myDataType *item_B) {
//    printf("Swap 1 time\n");
    myDataType item_tmp;
    item_tmp.key_part1=(*item_A).key_part1;
    item_tmp.key_part2=(*item_A).key_part2;
    item_tmp.key_part3=(*item_A).key_part3;
    item_tmp.key_part4=(*item_A).key_part4;
    item_tmp.key_part5=(*item_A).key_part5;
    item_tmp.key_part6=(*item_A).key_part6;
    item_tmp.key_part7=(*item_A).key_part7;
    item_tmp.key_part8=(*item_A).key_part8;
    item_tmp.data_part1=(*item_A).data_part1;
    item_tmp.data_part2=(*item_A).data_part2;
    item_tmp.data_part3=(*item_A).data_part3;
    item_tmp.data_part4=(*item_A).data_part4;
    item_tmp.reserved_part1=(*item_A).reserved_part1;
    item_tmp.reserved_part2=(*item_A).reserved_part2;
    item_tmp.flag_channel=(*item_A).flag_channel;
    item_tmp.flag_end=(*item_A).flag_end;

    (*item_A).key_part1=(*item_B).key_part1;
    (*item_A).key_part2=(*item_B).key_part2;
    (*item_A).key_part3=(*item_B).key_part3;
    (*item_A).key_part4=(*item_B).key_part4;
    (*item_A).key_part5=(*item_B).key_part5;
    (*item_A).key_part6=(*item_B).key_part6;
    (*item_A).key_part7=(*item_B).key_part7;
    (*item_A).key_part8=(*item_B).key_part8;
    (*item_A).data_part1=(*item_B).data_part1;
    (*item_A).data_part2=(*item_B).data_part2;
    (*item_A).data_part3=(*item_B).data_part3;
    (*item_A).data_part4=(*item_B).data_part4;
    (*item_A).reserved_part1=(*item_B).reserved_part1;
    (*item_A).reserved_part2=(*item_B).reserved_part2;
    (*item_A).flag_channel=(*item_B).flag_channel;
    (*item_A).flag_end=(*item_B).flag_end;


    (*item_B).key_part1=item_tmp.key_part1;
    (*item_B).key_part2=item_tmp.key_part2;
    (*item_B).key_part3=item_tmp.key_part3;
    (*item_B).key_part4=item_tmp.key_part4;
    (*item_B).key_part5=item_tmp.key_part5;
    (*item_B).key_part6=item_tmp.key_part6;
    (*item_B).key_part7=item_tmp.key_part7;
    (*item_B).key_part8=item_tmp.key_part8;
    (*item_B).data_part1=item_tmp.data_part1;
    (*item_B).data_part2=item_tmp.data_part2;
    (*item_B).data_part3=item_tmp.data_part3;
    (*item_B).data_part4=item_tmp.data_part4;
    (*item_B).reserved_part1=item_tmp.reserved_part1;
    (*item_B).reserved_part2=item_tmp.reserved_part2;
    (*item_B).flag_channel=item_tmp.flag_channel;
    (*item_B).flag_end=item_tmp.flag_end;
}

double f_get_time() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec+(double)tv.tv_usec*1e-6;
}

