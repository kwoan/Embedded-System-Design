#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

volatile int segment(int);              // segment number display code
volatile int segment_op(char);          // segment operator display code
int wait_for_released(int, char*);      // polling key for released
int click_count(int, char*);            // click / double click detect

char operator;                          // 사칙연산자
char operator_eq;                       // 등호

int main(int argc, char* argv[]){
        int hex3_hex0, hex5_hex4, key, ledr;    // device file descriptor

        int input1 = 0; int input2 = 0; int output = 0;         // operand, output value

        int hex3_hex0_data = 0; int hex5_hex4_data = 0;         // hex에 display될 값
        char key_data; char ledr_data;                          // key에서 읽을 값 / ledr에 display될 값

        int stage=0; int op_sel=0;                              // 현재 stage 값 / 수행할 연산을 결정하는 값

        int pressed_count;                                      // key가 click된 횟수 (1 혹은 2)


        
        hex3_hex0 = open("/dev/hex3_hex0", O_WRONLY);           // device file open
        hex5_hex4 = open("/dev/hex5_hex4", O_WRONLY);
        key = open("/dev/key", O_RDONLY);
        ledr= open("/dev/ledr",O_WRONLY);
        while(1){                                               

                hex3_hex0_data = segment(output) | segment_op(operator_eq)<<8 | \
                                segment(input2)<<16 | segment_op(operator)<<24;         // set hex3-0 display value
                hex5_hex4_data = segment(input1);                                       // set hex5-4 display value
                ledr_data = (int)pow(2, op_sel);                                        // set ledr display value 

                write(hex3_hex0, &hex3_hex0_data, 4);           // hex3_hex0에 data write                               
                write(hex5_hex4, &hex5_hex4_data, 4);           // hex5_hex4에 data write
                read(key, &key_data, 1);                        // key에서 data read
                write(ledr, &ledr_data, 1);                     // ledr에 data write

                if      ((key_data & 0x1)!=0){                  // key0 check 

                        if      (stage==0){                     // stage0 : get input1

                                pressed_count = click_count(key, &key_data);    // click / double click check
                                if(pressed_count==1) input1++;                  // get input1
                                if(pressed_count==2) stage = (stage + 1) % 4;   // Next stage

                        }
                        else if (stage==1){                     // stage1 : get input2

                                pressed_count = click_count(key, &key_data);    // click / double click check
                                if(pressed_count==1) input2++;                  // get input2
                                if(pressed_count==2) stage = (stage + 1) % 4;   // Next stage

                        }
                        else if (stage==2){                     // stage2 : get operator
                        
                                pressed_count = click_count(key, &key_data);    // click / double click check
                                if(pressed_count==1) op_sel = (op_sel + 1) % 4; // operator select
                                if(pressed_count==2) {                          // selected operation 실행, operator display
                                        switch(op_sel){
                                                case 0: output = input1 + input2; operator = '+'; break;   
                                                case 1: output = input1 - input2; operator = '-'; break;
                                                case 2: output = input1 * input2; operator = '*'; break;
                                                case 3: output = input1 / input2; operator = '/'; break;
                                        }
                                        operator_eq = '=';                      // 등호 display
                                        stage = (stage + 1) % 4;                // Next stage
                                }
                        }

                        else if (stage==3){                     // stage3 : clear

                                pressed_count = click_count(key, &key_data);    // click / double click check
                                if(pressed_count==2) {                          //clear
                                        input1 = 0; input2 = 0;  output = 0; 
                                        operator = 0; operator_eq = 0; op_sel=0;
                                        stage = (stage + 1) % 4;                // Next stage
                                }
                        }
                }
                
        }
        return 0;
}

volatile int segment(int num){
        volatile int seg_value;
        seg_value =     (num<=0) ? 0x3f :\
                        (num==1) ? 0x06 :\
                        (num==2) ? 0x5b :\
                        (num==3) ? 0x4f :\
                        (num==4) ? 0x66 :\
                        (num==5) ? 0x6d :\
                        (num==6) ? 0x7d :\
                        (num==7) ? 0x07 :\
                        (num==8) ? 0x7f : 0x6f;
        return seg_value;
}

volatile int segment_op(char op){
        volatile int seg_value;
        seg_value =     (op=='+') ? 0b1110011 :\
                        (op=='-') ? 0b1000000 :\
                        (op=='*') ? 0b1110110 :\
                        (op=='/') ? 0b1011110 :\
                        (op=='=') ? 0b0001001 : 0x0;
        return seg_value;
}

int wait_for_released(int fd, char* key_data){          

        int pressed_count=0;
        while(1){
                read(fd, key_data, 1);          // polling key
                if(*key_data==0) {pressed_count=1; break;}      // if key released, pressed_count = 1 ==> break
        }
        return pressed_count;                   // pressed_count return 
}

int click_count(int fd, char* key_data){

        int cnt=50000;                                  // 실험적으로 결정된 값
        int pressed_count=0;
        pressed_count=wait_for_released(fd, key_data);  // key pressed event 발생
        while(cnt>0){                                   // 일정 시간 내에 key0 pressed event가 다시 발생하는 지 확인
                read(fd, key_data, 1);                  // polling key
                if(*key_data==1) {                      // key0 check
                        pressed_count += wait_for_released(fd, key_data);      // pressed_count++ ==> break
                        break;
                }
                cnt--;
        }
        return pressed_count;                           // key pressed event가 발생한 횟수 return
}

