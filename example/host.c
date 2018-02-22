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
#define DATAWIDTH 16 //Bytes
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

typedef struct {
    myDataType* data_to_fpga;
    int k;
    int item_HW;
    int loop_cnt;
    int* sent_cnt;
    myDataType** p_channel;
    int* received_cnt;

    int fpga_write_driver_fd;
} mySendTreadType;

typedef struct {
    myDataType* data_from_fpga;
    int k;
    int item_HW;
    int loop_cnt;
    int* received_cnt;
} myReceiveTreadType;

typedef struct {
    myDataType* data_to_cpu;
    int start_index;
    int end_index;
} myQuickSortTreadType;

void f_td_fpga_read(myReceiveTreadType* rtt);
void* f_td_fpga_write(void* arg);
void* f_td_quick_sort(void* arg);

void f_add_flag(int loop_cnt, int k, int item_HW, myDataType* data_to_fpga);
void f_write_item(int fpga_write_driver_fd, int item_num, myDataType* data_to_fpga);
void f_read_item(int fpga_read_driver_fd, myReceiveTreadType* rtt);

void f_quick_sort(myDataType* data_to_cpu, int start_index, int end_index);
int f_get_data(myDataType* item);
void f_assign_A_with_B(myDataType* item_A, myDataType* item_B);
void f_swap(myDataType* item_A, myDataType* item_B);

double f_get_time();
int f_log(double base, double x);
int f_2base_judgement(int x);

int ch_fifo_size, read_buf_size, write_buf_size, write_ch_buf_size;
double empty_percent;

double time_program_start;

double t_time_start;
const char *devWrite = NULL; 
const char *devRead = NULL;
const char *devLog = NULL;
FILE *fp_log;

void sleep_ms(int milliseconds) // cross-platform sleep function
{
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int main(int argc, char* argv[])
{
    myDataType* data_result;
    myDataType* data_to_fpga;
    myDataType* data_from_fpga;
    myDataType* data_to_cpu;
    myDataType* p_channel[K];
    myDataType* data_tmp;

    mySendTreadType stt;
    myReceiveTreadType rtt;
    myQuickSortTreadType qtt;
    FILE* fp_read;
    FILE* fp_write;
    pthread_t td_read_from_fpga;
    pthread_t td_write_to_fpga;
    pthread_t td_quick_sort;
    int k; //k-way merge tree
    int item_total_num, item_HW, item_SW;
    int loop_time;
    int loop_cnt = 1;
    int* sent_cnt;
    int* received_cnt;
    int real_item_read_num;
    double time_read_start, time_read_end, time_write_start, time_write_end;
    double *time_start_loop, *time_end_loop, *time_end_soft;
    int index, i;
    sent_cnt = (int*)calloc(K, sizeof(int));
    received_cnt = (int*)calloc(K, sizeof(int));
    int fpga_write_driver_fd;
    int sleep_at_start;

    time_program_start = f_get_time();

    ch_fifo_size = atoi(argv[5]) * DATAWIDTH;
    read_buf_size = atoi(argv[6]) * DATAWIDTH;
    empty_percent = atof(argv[7]);

    devRead = argv[8];
    devWrite = argv[9];
    devLog = argv[10];
    sleep_at_start = atoi(argv[11]);
    fp_log = fopen(devLog,"w");   
    sleep_ms(sleep_at_start); 
    fprintf(fp_log,"r,%s,%.6f,0\n", devRead, f_get_time());

    //-------------------------------------------------
    // Check item_total_num is bigger than 0
    //-------------------------------------------------
    item_total_num = atoi(argv[1]);
    if (item_total_num <= 0) {
        printf("Item number must bigger than 0.\n");
        return 0;
    }
    data_result = (myDataType*)calloc(item_total_num, sizeof(myDataType));
    data_to_fpga = (myDataType*)calloc(item_total_num, sizeof(myDataType));
    data_from_fpga = (myDataType*)calloc(item_total_num, sizeof(myDataType));
    //-------------------------------------------------
    // Check k is base-2 number
    //-------------------------------------------------
    k = atoi(argv[2]);
    if (f_2base_judgement(k) == 0) {
        printf("k must be base-2 number.\n");
        return 0;
    }
    //-------------------------------------------------
    // Open data file
    //-------------------------------------------------
    fp_read = fopen(argv[3], "r");
    if (fp_read == NULL) {
        printf("%s open failed: %s.\n", argv[3], strerror(errno));
        return EXIT_FAILURE;
    }
    //-------------------------------------------------
    // Read data to memory
    //-------------------------------------------------
    time_read_start = f_get_time();

    real_item_read_num = fread(data_to_fpga, sizeof(myDataType), item_total_num, fp_read);
    
    if (real_item_read_num < item_total_num) {
        printf("Real read item number is less than appointed number.\n");
        return 0;
    }
    printf("There are %d items in this file.\n", item_total_num);
    time_read_end = f_get_time();
    printf("Read time is %.6f s.\n", time_read_end - time_read_start);

    //------------------------------------------------------
    // Sperate items to HW sorting part and SW sorting part
    //------------------------------------------------------
    loop_time = f_log(k, (double)item_total_num);
    //    loop_time=1;
    item_HW = (int)pow((double)k, (double)loop_time);
    item_SW = item_total_num - item_HW;

    data_to_cpu = data_to_fpga + item_HW;
    printf("item_HW=%d.\n", item_HW);
    printf("item_SW=%d.\n", item_SW);
    //------------------------------------------------------
    // Prepare for quick sort thread
    //------------------------------------------------------
    qtt.data_to_cpu = data_to_cpu;
    qtt.start_index = 0;
    qtt.end_index = item_SW - 1;

    //------------------------------------------------------
    // Start quick sort
    //------------------------------------------------------
    pthread_create(&td_quick_sort, NULL, f_td_quick_sort, &qtt);

    //------------------------------------------------------
    // Prepare for HW send and receive thread
    //------------------------------------------------------
    stt.data_to_fpga = data_to_fpga;
    stt.k = k;
    stt.item_HW = item_HW;
    rtt.data_from_fpga = data_from_fpga;
    rtt.k = k;
    rtt.item_HW = item_HW;

    time_start_loop = (double*)calloc(loop_time, sizeof(double));
    time_end_loop = (double*)calloc(loop_time, sizeof(double));
    time_end_soft = (double*)calloc(loop_time, sizeof(double));
    
    //f_add_flag(loop_cnt, k, item_HW, stt.data_to_fpga);

    t_time_start = f_get_time();
    fprintf(fp_log,"r,%s,%.6f,0\n", devRead, f_get_time());
    
    while (loop_cnt <= loop_time) {
        time_start_loop[loop_cnt - 1] = f_get_time();
        printf("Loop time %d.\n", loop_time);
        stt.loop_cnt = loop_cnt;
        rtt.loop_cnt = loop_cnt;
        for (index = 0; index < k; index++) {
            sent_cnt[index] = 0;
            received_cnt[index] = 0;
            p_channel[index] = &stt.data_to_fpga[index * ((int)(item_HW / k))];
        }
        stt.sent_cnt = sent_cnt;
        stt.received_cnt = received_cnt;
        stt.p_channel = p_channel;
        rtt.received_cnt = received_cnt;
        
        f_add_flag(loop_cnt, k, item_HW, stt.data_to_fpga);
        	
        time_end_soft[loop_cnt - 1] = f_get_time();

        pthread_create(&td_write_to_fpga, NULL, f_td_fpga_write, &stt);
	      f_td_fpga_read(&rtt);

        data_tmp = rtt.data_from_fpga;
        rtt.data_from_fpga = stt.data_to_fpga;
        stt.data_to_fpga = data_tmp;

        time_end_loop[loop_cnt - 1] = f_get_time();
        printf("Software process time is %.6f s.\n", time_end_soft[loop_cnt - 1] - time_start_loop[loop_cnt - 1]);
        printf("Hardware process time is %.6f s.\n", time_end_loop[loop_cnt - 1] - time_end_soft[loop_cnt - 1]);
        printf("Loop %d time is %.6f s.\n\n", loop_cnt, time_end_loop[loop_cnt - 1] - time_start_loop[loop_cnt - 1]);

        loop_cnt++;
    }

    printf("[t][%.6f][%s] end sort\n", f_get_time() - t_time_start, devRead);
    data_to_fpga = rtt.data_from_fpga;
    data_from_fpga = stt.data_to_fpga;
    pthread_join(td_quick_sort, NULL);

    //------------------------------------------------------
    // Merge SW and HW parts
    //------------------------------------------------------
    int hw_cnt = 0, sw_cnt = 0;
    for (index = 0; index < item_total_num; index++) {
        if (hw_cnt < item_HW && sw_cnt < item_SW) {
            if (f_get_data(&data_from_fpga[hw_cnt]) < f_get_data(&data_to_cpu[sw_cnt])) {
                f_assign_A_with_B(&data_result[index], &data_from_fpga[hw_cnt]);
                hw_cnt++;
            }
            else {
                f_assign_A_with_B(&data_result[index], &data_to_cpu[sw_cnt]);
                sw_cnt++;
            }
        }
        else if (hw_cnt == item_HW) {
            f_assign_A_with_B(&data_result[index], &data_to_cpu[sw_cnt]);
            sw_cnt++;
        }
        else if (sw_cnt == item_SW) {
            f_assign_A_with_B(&data_result[index], &data_from_fpga[hw_cnt]);
            hw_cnt++;
        }
    }

    //------------------------------------------------------
    // Sort completed, write the result
    //------------------------------------------------------
    fp_write = fopen(argv[4], "w");
    if (fp_write == NULL) {
        printf("%s open failed: %s.\n", argv[4], strerror(errno));
        return EXIT_FAILURE;
    }

    time_write_start = f_get_time();
    printf("Sorting time is %.6f s.\n", time_write_start - time_read_end);
    fwrite(data_result, sizeof(myDataType), item_total_num, fp_write);
    time_write_end = f_get_time();
    printf("Write time is %.6f s.\n", time_write_end - time_write_start);
    double ave_HW_process_time = 0;
    for (index = 0; index < loop_time; index++) {
        ave_HW_process_time += (time_end_loop[index] - time_end_soft[index]);
    }
    ave_HW_process_time /= loop_time;
    printf("Average hardware process time in loop is %.6f s.\n", ave_HW_process_time);
    free(data_result);
    free(data_to_fpga);
    free(data_from_fpga);
    fclose(fp_read);
    fclose(fp_write);
    fprintf(fp_log,"r,%s,%.6f,0\n", devRead, f_get_time());
    return 0;
}

void f_add_flag(int loop_cnt, int k, int item_HW, myDataType* data_to_fpga)
{
    int index;
    int span;
    int channel = -1;
    span = (int)pow((double)k, (double)(loop_cnt - 1));
    printf("Span=%d.\n", span);
    for (index = 0; index < item_HW; index++) {
        if ((index + 1) % span == 0) {
            data_to_fpga[index].flag_end = 0x01;
        }else{
            data_to_fpga[index].flag_end = 0x00;
        }
        if (index % (item_HW / k) == 0) {
            channel++;
        }
        data_to_fpga[index].flag_channel = (unsigned char)channel;
    }
    printf("Flag addition finish.\n");
}


void f_write_item(int fpga_write_driver_fd, int item_num, myDataType* data_to_fpga)
{
    unsigned char* buf;
    int index, i;
    int bytes_cnt = 0;
    int bytes_written = 0;
    int sent_cnt = 0;
    int item_in_buff_this_time;

    double time_start, time_end;

    time_start = f_get_time();
    while (sent_cnt < item_num * DATAWIDTH) {
        bytes_written = write(fpga_write_driver_fd, ((unsigned char*)(data_to_fpga)) + sent_cnt, item_num * DATAWIDTH - sent_cnt);

        if ((bytes_written < 0) && (errno == EINTR)) {
            printf("here(?!).\n");
            continue;
        }
        if (bytes_written < 0) {
            printf("write failed, %d bytes haven't written.\n", item_num * DATAWIDTH - (bytes_cnt + sent_cnt));
            continue;
        }
        if (bytes_written == 0) {
            break;
        }
        sent_cnt += bytes_written;
    }
    time_end= f_get_time();

    //printf("sent:%d B, used time: %.6f \n", item_num * 16, time_end - time_start);

    free(buf);
}


void* f_td_fpga_write(void* arg)
{
    mySendTreadType* stt = arg;
    myDataType* data_to_fpga = (*stt).data_to_fpga;

    int k = (*stt).k;
    int loop_cnt = (*stt).loop_cnt;
    int* sent_cnt = (*stt).sent_cnt;
    int* received_cnt = (*stt).received_cnt;
    int item_HW = (*stt).item_HW;
    myDataType** p_channel = (*stt).p_channel;
    int index;
    int i;
    int sent_tmp,sent_tmp2;
    int fpga_write_driver_fd = (*stt).fpga_write_driver_fd;
    myDataType* data_to_send;
    int data_to_send_cnt, data_order_cnt;
    int sent_sum;
    
    lbopenwrite:
    fpga_write_driver_fd = open(devWrite, O_WRONLY);
    if (fpga_write_driver_fd < 0) {
        goto lbopenwrite;
    }
    (*stt).fpga_write_driver_fd = fpga_write_driver_fd;

    printf("Start writting thread.\n");
    printf("loop_cnt=%d,k=%d,item_HW=%d.\n", loop_cnt, k, item_HW);

    data_to_send = (myDataType*)calloc(ch_fifo_size * DATAWIDTH * k, sizeof(myDataType));

    double time_start, time_start_hw;
    double time_sw, time_hw;
    time_hw = 0;
    time_sw = 0;
    time_start = f_get_time();

    while (sent_sum < item_HW) {
        data_to_send_cnt = 0;
        for (index = 0; index < k; index++) {
            sent_tmp = ch_fifo_size / DATAWIDTH - (sent_cnt[index] - received_cnt[index]);
            sent_tmp2 = (sent_cnt[index] + sent_tmp) <= (int)(item_HW / k) ? sent_tmp : ((int)(item_HW / k) - sent_cnt[index]);
            if (sent_tmp < (ch_fifo_size / DATAWIDTH) * empty_percent && (sent_tmp == sent_tmp2))
                continue;
                
            sent_tmp = sent_tmp2;

    	      for(i = 0; i < sent_tmp; i++){
    		      data_to_send[data_to_send_cnt + i] = p_channel[index][i]; 
    	      }

            data_to_send_cnt += sent_tmp;
            sent_cnt[index] += sent_tmp;
            p_channel[index] += sent_tmp;
        }
        
        if(data_to_send_cnt == 0)
            continue;

	    	time_sw += (f_get_time() - time_start);

        time_start_hw = f_get_time();
        f_write_item(fpga_write_driver_fd, data_to_send_cnt, data_to_send);
        time_hw += (f_get_time() - time_start_hw);

        sent_sum = 0;
        for (index = 0; index < k; index++) {
            sent_sum += sent_cnt[index];
        }
    }
    time_sw = f_get_time() - time_start - time_hw;
    printf("time sw: %.6fs, time hw: %.6fs \n", time_sw, time_hw);

    free(data_to_send);
    close(fpga_write_driver_fd);
    printf("FPGA write finish.\n");
}

void f_read_item(int fpga_read_driver_fd, myReceiveTreadType* rtt)
{
    unsigned char* buf;
    int item_cnt = 0;
    int bytes_cnt = 0;
    int bytes_read = 0;
    int index;
    int item_index = 0;
    int span = 0;
    int channel = -1;
    double time_start_read; 

    buf = (unsigned char*)calloc(read_buf_size, sizeof(unsigned char));
    span = (int)pow((double)(*rtt).k, (double)((*rtt).loop_cnt) - 1);
    printf("read span %d\n", span);
    
    while (1) { 
        time_start_read = f_get_time();
        bytes_read = read(fpga_read_driver_fd, buf, read_buf_size);
        if ((bytes_read < 0) && (errno == EINTR)) {
            continue;
        }
        if (bytes_read < 0) {
            printf("Read failed.\n");
            exit(1);
        }
        if (bytes_read == 0) {
            continue;
        }
        
        memcpy((unsigned char*)(*rtt).data_from_fpga + item_cnt, buf, bytes_read);

        for (index = 0; index < bytes_read; index += 16) {
            (*rtt).received_cnt[buf[1 + index]] += 1;
     /*       
            if ((item_index + 1) % span == 0) {
				        (*rtt).data_from_fpga[item_index].flag_end = 0x01;
				    }else{
				        (*rtt).data_from_fpga[item_index].flag_end = 0x00;
				    }
				    if (item_index % ((*rtt).item_HW / (*rtt).k) == 0) {
				        channel++;
				    }
				    (*rtt).data_from_fpga[item_index].flag_channel = (unsigned char)channel;
            
            item_index++;*/
        }
        
        item_cnt += bytes_read;
        fprintf(fp_log, "t,%s,%.6f,%f\n", devRead, f_get_time(), (bytes_read/1024.0/1024.0/(f_get_time() - time_start_read)));
        if (item_cnt / DATAWIDTH >= (*rtt).item_HW)
            break;
    }
    free(buf);
}

void f_td_fpga_read(myReceiveTreadType* rtt)
{
    int fpga_read_driver_fd;
    printf("Start reading thread.\n");
    lbopenread:
    fpga_read_driver_fd = open(devRead, O_RDONLY);
    if (fpga_read_driver_fd < 0) {
        goto lbopenread;
    }
    f_read_item(fpga_read_driver_fd, rtt);
    close(fpga_read_driver_fd);
}

void* f_td_quick_sort(void* arg)
{
    myQuickSortTreadType* qtt = arg;
    myDataType* data_to_cpu = (*qtt).data_to_cpu;
    int start_index = (*qtt).start_index;
    int end_index = (*qtt).end_index;
    f_quick_sort(data_to_cpu, start_index, end_index);
}

void f_quick_sort(myDataType* data_to_cpu, int start_index, int end_index)
{
    int i = start_index, j = end_index;
    int pivot;
    pivot = f_get_data(&data_to_cpu[(start_index + end_index) / 2]);
    while (1) {
        while (f_get_data(&data_to_cpu[i]) < pivot)
            i++;
        while (f_get_data(&data_to_cpu[j]) > pivot)
            j--;
        if (i >= j)
            break;
        f_swap(&data_to_cpu[i], &data_to_cpu[j]);
        i++;
        j--;
    }
    if (start_index < i - 1)
        f_quick_sort(data_to_cpu, start_index, i - 1);
    if (j + 1 < end_index)
        f_quick_sort(data_to_cpu, j + 1, end_index);
}

int f_get_data(myDataType* item)
{
    int item_value = 0;
    item_value |= (int)(*item).data_part1 << 8 * 3;
    item_value |= (int)(*item).data_part2 << 8 * 2;
    item_value |= (int)(*item).data_part3 << 8 * 1;
    item_value |= (int)(*item).data_part4 << 8 * 0;
    return item_value;
}
void f_assign_A_with_B(myDataType* item_A, myDataType* item_B)
{
    (*item_A).key_part1 = (*item_B).key_part1;
    (*item_A).key_part2 = (*item_B).key_part2;
    (*item_A).key_part3 = (*item_B).key_part3;
    (*item_A).key_part4 = (*item_B).key_part4;
    (*item_A).key_part5 = (*item_B).key_part5;
    (*item_A).key_part6 = (*item_B).key_part6;
    (*item_A).key_part7 = (*item_B).key_part7;
    (*item_A).key_part8 = (*item_B).key_part8;
    (*item_A).data_part1 = (*item_B).data_part1;
    (*item_A).data_part2 = (*item_B).data_part2;
    (*item_A).data_part3 = (*item_B).data_part3;
    (*item_A).data_part4 = (*item_B).data_part4;
/*
    (*item_A).reserved_part1 = (*item_B).reserved_part1;
    (*item_A).reserved_part2 = (*item_B).reserved_part2;
    (*item_A).flag_channel = (*item_B).flag_channel;
    (*item_A).flag_end = (*item_B).flag_end;
*/

    (*item_A).reserved_part1 = 0x00;
    (*item_A).reserved_part2 = 0x00;
    (*item_A).flag_channel = 0x00;
    (*item_A).flag_end = 0x00;
}

void f_swap(myDataType* item_A, myDataType* item_B)
{
    myDataType item_tmp;
    item_tmp.key_part1 = (*item_A).key_part1;
    item_tmp.key_part2 = (*item_A).key_part2;
    item_tmp.key_part3 = (*item_A).key_part3;
    item_tmp.key_part4 = (*item_A).key_part4;
    item_tmp.key_part5 = (*item_A).key_part5;
    item_tmp.key_part6 = (*item_A).key_part6;
    item_tmp.key_part7 = (*item_A).key_part7;
    item_tmp.key_part8 = (*item_A).key_part8;
    item_tmp.data_part1 = (*item_A).data_part1;
    item_tmp.data_part2 = (*item_A).data_part2;
    item_tmp.data_part3 = (*item_A).data_part3;
    item_tmp.data_part4 = (*item_A).data_part4;
    item_tmp.reserved_part1 = (*item_A).reserved_part1;
    item_tmp.reserved_part2 = (*item_A).reserved_part2;
    item_tmp.flag_channel = (*item_A).flag_channel;
    item_tmp.flag_end = (*item_A).flag_end;

    (*item_A).key_part1 = (*item_B).key_part1;
    (*item_A).key_part2 = (*item_B).key_part2;
    (*item_A).key_part3 = (*item_B).key_part3;
    (*item_A).key_part4 = (*item_B).key_part4;
    (*item_A).key_part5 = (*item_B).key_part5;
    (*item_A).key_part6 = (*item_B).key_part6;
    (*item_A).key_part7 = (*item_B).key_part7;
    (*item_A).key_part8 = (*item_B).key_part8;
    (*item_A).data_part1 = (*item_B).data_part1;
    (*item_A).data_part2 = (*item_B).data_part2;
    (*item_A).data_part3 = (*item_B).data_part3;
    (*item_A).data_part4 = (*item_B).data_part4;
    (*item_A).reserved_part1 = (*item_B).reserved_part1;
    (*item_A).reserved_part2 = (*item_B).reserved_part2;
    (*item_A).flag_channel = (*item_B).flag_channel;
    (*item_A).flag_end = (*item_B).flag_end;

    (*item_B).key_part1 = item_tmp.key_part1;
    (*item_B).key_part2 = item_tmp.key_part2;
    (*item_B).key_part3 = item_tmp.key_part3;
    (*item_B).key_part4 = item_tmp.key_part4;
    (*item_B).key_part5 = item_tmp.key_part5;
    (*item_B).key_part6 = item_tmp.key_part6;
    (*item_B).key_part7 = item_tmp.key_part7;
    (*item_B).key_part8 = item_tmp.key_part8;
    (*item_B).data_part1 = item_tmp.data_part1;
    (*item_B).data_part2 = item_tmp.data_part2;
    (*item_B).data_part3 = item_tmp.data_part3;
    (*item_B).data_part4 = item_tmp.data_part4;
    (*item_B).reserved_part1 = item_tmp.reserved_part1;
    (*item_B).reserved_part2 = item_tmp.reserved_part2;
    (*item_B).flag_channel = item_tmp.flag_channel;
    (*item_B).flag_end = item_tmp.flag_end;
}

double f_get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + (double)tv.tv_usec * 1e-6;
}

int f_log(double base, double x)
{
    return (int)(log(x) / log(base));
}

int f_2base_judgement(int x)
{
    if ((x & (x - 1)) == 0)
        return 1;
    else
        return 0;
}
