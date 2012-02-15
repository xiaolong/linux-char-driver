KDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

obj-m:= simplechar.o
default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	$(RM) *.o *.ko *.mod.c Module.symvers modules.order
