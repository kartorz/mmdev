
ifneq ($(KERNELRELEASE),)
obj-m := winemm.o
winemm.y = mmdev.o

else
CFLAGS := "-O0"
KNLBLD= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KNLBLD) M=$(PWD) modules

clean:
	@rm *.ko *.mod.c *.o  modules.* Module.*
endif
