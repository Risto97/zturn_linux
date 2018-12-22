export ARCH:=arm
export CROSS_COMPILE:=arm-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc

obj-m := dm_cmd.o

KERNELDIR := ../../linux-xlnx/
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm -f *~
