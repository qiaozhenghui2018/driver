
ifeq ($(KERNELRELEASE), )
	KERNELDIR ?=/home/qiaozhh/linux_kernel/linux-4.6.6/
	PWD       := $(shell pwd)
	EXTRA_CFLAGS += -v -I $(obj)/../include -I(KERNELDIR)/private/add_sub/
	KBUILD_EXTRA_SYMBOLS=$(obj)/../print/Module.symvers
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.o *~ core .*.cmd *.mod.c ./tmp_version
else
	obj-m := add_sub_test.o
endif
