obj-m += key_dd.o
obj-m += ledr_dd.o
obj-m += hex3_hex0_dd.o
obj-m += hex5_hex4_dd.o

all:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
