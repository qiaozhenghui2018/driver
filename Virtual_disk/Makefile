
ifeq ($(KERNELRELEASE), )
	KERNELDIR ?=/home/joe/linux_kernel/linux-4.6.6/
	PWD       := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .*.cmd *.mod.c ./tmp_version
else
	obj-m := virtual_disk.o
endif
