SRC := src/servidorinterface
BUILD := build
DIST := dist
CLASSES := $(BUILD)/classes
FX_PATH := ./libFX/javaFX-11/lib
JBUILD := servidorinterface.

all: compile

compile: $(SRC)/ServidorInterface.java $(SRC)/ServidorInterfaceDocumentController.java $(SRC)/Servidor.java
	javac --module-path $(FX_PATH) --add-modules javafx.controls,javafx.fxml -d $(CLASSES)  $(SRC)/ServidorInterface.java $(SRC)/ServidorInterfaceDocumentController.java $(SRC)/Servidor.java

start: 
	cd $(CLASSES) && java --module-path ../../$(FX_PATH) --add-modules javafx.controls,javafx.fxml  $(JBUILD)ServidorInterface $(JBUILD)ServidorInterfaceDocumentController $(JBUILD)Servidor

clean: $(CLASSES)/servidorinterface $(DIST)/*
	rm -r $(CLASSES)/servidorinterface && rm -I $(DIST)/*