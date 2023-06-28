file_src_name := WhisperDriver
obj-m := $(file_src_name).o

all: $(file_src_name).c
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	
clean: $(file_src_name).ko
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	
insert: $(file_src_name).ko
	sudo insmod $(file_src_name).ko

remove:
	sudo rmmod $(file_src_name).ko

vlog:
	sudo dmesg --follow