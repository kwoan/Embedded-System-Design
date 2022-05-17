#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define LW_BRIDGE_SPAN 0x00005000
#define LW_BRIDGE_BASE 0xFF200000
#define HEX_BASE_3_0   0x00000020
#define HEX_BASE_5_4   0x00000030
#define KEY_BASE       0x00000050
#define LEDR_BASE      0x00000000

int segment(int a){
        int b=  (a<=0)? 0x3f:\
                (a==1)? 0x06:\
                (a==2)? 0x5b:\
                (a==3)? 0x4f:\
                (a==4)? 0x66:\
                (a==5)? 0x6d:\
                (a==6)? 0x7d:\
                (a==7)? 0x07:\
                (a==8)? 0x7f:0x6f;
        return b;
}

int plus(int input1, int input2){
        int tmp=0;
        tmp = input1 + input2;
        return tmp;
}

int minus(int input1, int input2){
        int tmp=0;
        tmp = input1 - input2;
        return tmp;
}

int mul(int input1, int input2){
        int tmp=0;
        tmp = input1 * input2;
        return tmp;
}

int div(int input1, int input2){
        int tmp =0;
        tmp = input1 / input2;
        return tmp;
}


int wait_for_value(volatile int* key, volatile int* ledr){
        int cnt=0;
        while(1){
                if(((*key)&0x1)==0) break;

                cnt++;
                *ledr = cnt;
                usleep(1000*1000);
        }
        if(cnt>9) 	cnt=9;
        return cnt;
}

int wait_for_operation(volatile int* key, volatile int* ledr, \
                        int input1, int input2){
        int cnt=0;
        int tmp=0;
        while(1){
                if(((*key)&0x1)==0) break;

                cnt++;
                if(cnt==5) cnt=1;
                *ledr=cnt;
                usleep(1000*1000);
        }
        if		(cnt==1) 		tmp = plus(input1, input2);
        else if	(cnt==2)	 	tmp = minus(input1, input2);
        else if	(cnt==3) 		tmp = mul(input1, input2);
        else if	(cnt==4) 		tmp = div(input1, input2);
        return tmp;
}

int main(void){
        int fd;
        void* lw_virtual;
        volatile int* key;
        volatile int* hex_3_0;
        volatile int* hex_5_4;
        volatile int* ledr;

        fd = open("/dev/mem", (O_RDWR | O_SYNC));
        lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), \
                        MAP_SHARED, fd, LW_BRIDGE_BASE);
        key 	= (volatile int*) (lw_virtual + KEY_BASE);
        hex_3_0 = (volatile int*) (lw_virtual + HEX_BASE_3_0);
        hex_5_4 = (volatile int*) (lw_virtual + HEX_BASE_5_4);
        ledr    = (volatile int*) (lw_virtual + LEDR_BASE);

        int input1=0;
        int input2=0;
        int cnt=0;
        int operator_en=0;
        int input1_en=1;
        int input2_en=0;
        int output=0;

        while(1){
                *hex_3_0 = segment(output) | segment(input2)<<16;
                *hex_5_4 = segment(input1);
                if		((~operator_en) & (~input2_en) & (input1_en) & ((*key)!=0)){
                        input1 		= wait_for_value(key, ledr);
                        input1_en 	= 0;
                        input2_en 	= 1;
                }
                else if ((~operator_en) & (input2_en) & (~input1_en) & ((*key)!=0)){
                        input2 		= wait_for_value(key, ledr);
                        input2_en 	= 0;
                        operator_en = 1;
                }
                else if ((operator_en) & (~input2_en) & (~input1_en) & ((*key)!=0)){
                        output 		= wait_for_operation(key, ledr, input1, input2);
                        operator_en = 0;
                        input1_en 	= 1;
                }



        }

		munmap(lw_virtual, LW_BRIDGE_BASE);
		close(fd);
		
        return 0;
}
