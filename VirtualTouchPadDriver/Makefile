obj-m += virtual_touchpad.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


install:
	cp virtual_touchpad.ko /lib/modules/`uname -r`/kernel/drivers/input/mouse
	cp vtp-driver.init /etc/init.d/vtp-driver
	chmod +x /etc/init.d/vtp-driver
	update-rc.d vtp-driver defaults
	depmod -a
