SCR_MODULE := ./Kernel_Module
SCR_SEVER := ./ServidorInterface

all: compile

compile: $(SCR_MODULE)/Makefile $(SCR_SEVER)/Makefile
	$(SCR_MODULE)/Makefile all && $(SCR_SEVER)/Makefile all
	
insert: $(SCR_MODULE)/Makefile
	 $(SCR_MODULE)/Makefile insert

remove: $(SCR_MODULE)/Makefile
	 $(SCR_MODULE)/Makefile remove

clean: $(SCR_MODULE)/Makefile $(SCR_SEVER)/Makefile
	$(SCR_MODULE)/Makefile clean && $(SCR_SEVER)/Makefile clean
