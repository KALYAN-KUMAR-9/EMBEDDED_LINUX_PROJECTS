obj-m += 01_hello.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_ROOT_DIR=/home/kalyan-kumar/Desktop/BBB_Learning_WorkSpace/linux
HOST_KERN_ROOT_DIR=/lib/modules/$(shell uname -r)/build

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_ROOT_DIR) M=$(shell pwd) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_ROOT_DIR) M=$(shell pwd) clean

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_ROOT_DIR) M=$(shell pwd) help

host:
	make -C $(HOST_KERN_ROOT_DIR) M=$(shell pwd) modules
