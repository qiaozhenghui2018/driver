
ifeq ($(KERNELRELEASE), )
	KERNELDIR ?=/home/qiaozhh/linux_kernel/linux-4.6.6/
	PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .*.cmd *.mod.c ./tmp_version
else
	obj-m := add_sub.o
endif
