
ifneq ($(KERNELRELEASE),)
obj-m := mmdev.o

else
CFLAGS := "-O0"
KNLBLD= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KNLBLD) M=$(PWD) modules

clean:
	@rm *.ko *.mod.c *.o  modules.* Module.*
endif
