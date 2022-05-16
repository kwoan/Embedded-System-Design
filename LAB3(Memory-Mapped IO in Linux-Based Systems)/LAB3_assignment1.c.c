#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define LW_BRIDGE_SPAN 0x00005000
#define LW_BRIDGE_BASE 0xFF200000
#define LEDR_BASE      0x00000000
#define HEX_BASE_3_0   0x00000020
#define HEX_BASE_5_4   0x00000030

#define id_size 11
int  segment(int a)     {

        int b=  (a==0)? 0x3f:\
        (a==1)? 0x06:\
        (a==2)? 0x5b:\
        (a==3)? 0x4f:\
        (a==4)? 0x66:\
        (a==5)? 0x6d:\
        (a==6)? 0x7d:\
        (a==7)? 0x07:\
        (a==8)? 0x7f:\
		(a==9)? 0x6f:\
        (a==111)? 0x00:\
        0x0010000;

        return b;
}

void id_coding(int* id, int n){
        int i=0;
        for(i;i<n;i++){
                id[i] = segment(id[i]);
        }
}

int main(void){
        int fd;
        void* lw_virtual;
        int cnt=0;

        fd = open("/dev/mem", (O_RDWR | O_SYNC));
        lw_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ|PROT_WRITE), \
                        MAP_SHARED, fd, LW_BRIDGE_BASE);
        volatile int* hex_3_0 = (volatile int*)(lw_virtual + HEX_BASE_3_0);
		        volatile int* hex_5_4 = (volatile int*)(lw_virtual + HEX_BASE_5_4);

        int id[3*id_size] = {2,0,1,6,1,2,4,0,9,9,111,\
                        2,0,1,8,1,2,4,1,2,0,111,\
                        2,0,1,7,1,0,6,0,0,7,111};
        id_coding(id, 3*id_size);
        while(cnt<30){
                *hex_3_0 =      id[(cnt+5)%(3*id_size)] | \
                                (id[(cnt+4)%(3*id_size)]<<8) | \
                                (id[(cnt+3)%(3*id_size)]<<16) | \
                                (id[(cnt+2)%(3*id_size)]<<24);

                *hex_5_4 =      id[(cnt+1)%(3*id_size)] |\
                                (id[(cnt)%(3*id_size)]<<8);
                cnt++;
                usleep(1000000);
        }

        munmap(lw_virtual, LW_BRIDGE_BASE);
        close(fd);

        return 0;
}