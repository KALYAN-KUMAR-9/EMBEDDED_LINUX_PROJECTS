obj-m += 01_hello.o

# Default kernel directory if ARCH is set, otherwise fall back to host
ifdef ARCH
  KDIR = /home/kalyan-kumar/Desktop/BBB_Learning_WorkSpace/linux
endif

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=$(shell pwd) modules

clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=$(shell pwd) clean

