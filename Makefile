SCR_MODULE := ./Kernel_Module
SCR_SEVER := ./ServidorInterface

all: compile

compile: $(SCR_MODULE)/Makefile $(SCR_SEVER)/Makefile
	cd $(SCR_SEVER) && make jar
	cd $(SCR_MODULE) && make all
	
insert: $(SCR_MODULE)/Makefile
	 cd $(SCR_MODULE) && make insert

remove: $(SCR_MODULE)/Makefile
	 cd $(SCR_MODULE) && make remove

vlog: $(SCR_MODULE)/Makefile
	cd $(SCR_MODULE) && make vlog

start: $(SCR_SEVER)/Makefile
	cd $(SCR_SEVER) && make start

clean: $(SCR_MODULE)/Makefile $(SCR_SEVER)/Makefile
	cd $(SCR_SEVER) && make clean
	cd $(SCR_MODULE) && make clean 
