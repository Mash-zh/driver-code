.PHONY: all clean
#ifneq ($(KERNELRELEASE),)

obj-m := helloworld.o

#else

EXTRA_CFLAGS += -DDEBUG 
KDIR := /home/zhu/linux/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek
ARCH_ARGS := CROSS_COMPILE=arm-linux-gnueabihf- ARCH=arm

all:
		make $(ARCH_ARGS) -C $(KDIR) M=$(PWD) modules
clean:
		make $(ARCH_ARGS) -C $(KDIR) M=$(PWD) modules clean

#endif
