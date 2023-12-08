SRC := src/servidorinterface
BUILD := build
DIST := dist
CLASSES := $(BUILD)/classes
FX_PATH := libFX/javaFX-21/lib
JBUILD := servidorinterface.

all: compile

compile: $(SRC)/ServidorInterface.java $(SRC)/ServidorInterfaceDocumentController.java $(SRC)/Servidor.java $(SRC)/ServidorInterfaceDocument.fxml $(SRC)/styles.css 
	javac --module-path $(FX_PATH) --add-modules javafx.controls,javafx.fxml -d $(CLASSES)  $(SRC)/ServidorInterface.java $(SRC)/ServidorInterfaceDocumentController.java $(SRC)/Servidor.java
	cp $(SRC)/ServidorInterfaceDocument.fxml $(CLASSES)/servidorinterface
	cp $(SRC)/styles.css $(CLASSES)/servidorinterface

start: 
	cd $(CLASSES) && java --module-path ../../$(FX_PATH) --add-modules javafx.controls,javafx.fxml  $(JBUILD)ServidorInterface $(JBUILD)ServidorInterfaceDocumentController $(JBUILD)Servidor

clean: $(CLASSES)/servidorinterface
	rm -r $(CLASSES)/servidorinterface