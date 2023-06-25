obj-m := backdoorDriver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

insert:
	sudo insmod backdoorDriver.ko

remove:
	sudo rmmod backdoorDriver.ko

vlog:
	sudo dmesg --follow