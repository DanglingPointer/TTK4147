obj-m := mymodule.o
KERNELDIR := /home/student/Desktop/TTK4147/Ex10/buildroot-avr32-v3.0.0_TTK4147/output/build/linux-2.6.35.4
CROSS := /home/student/Desktop/TTK4147/Ex10/buildroot-avr32-v3.0.0_TTK4147/output/staging/usr/bin/avr32-linux-
all:
	make ARCH=avr32 CROSS_COMPILE=$(CROSS) -C $(KERNELDIR) M=$(shell pwd) modules
	cp mymodule.ko /export/nfs/root
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_version
