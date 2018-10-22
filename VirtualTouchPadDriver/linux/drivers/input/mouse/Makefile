KERNEL_VERSION :=$(shell uname -r)
KERNEL_MODULES :=/lib/modules/$(KERNEL_VERSION)/build
obj-m += virtual_touchpad.o

all:
	make -C $(KERNEL_MODULES) M=$(PWD) modules

clean:
	make -C $(KERNEL_MODULES) M=$(PWD) clean


install:
	cp virtual_touchpad.ko /lib/modules/`uname -r`/kernel/drivers/input/mouse
	cp vtp-driver.init /etc/init.d/vtp-driver
	chmod +x /etc/init.d/vtp-driver
	update-rc.d vtp-driver defaults
	depmod -a
