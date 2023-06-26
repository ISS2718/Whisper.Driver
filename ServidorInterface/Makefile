SRC := ./src/servidorinterface
BUILD := ./build
DIST := ./dist
CLASSES := $(BUILD)/classes

all: sstart

compile: $(SRC)/Servidor.java $(SRC)/ServidorInterface.java $(SRC)/ServidorInterfaceDocumentController.java $(SRC)/ServidorInterfaceDocument.fxml $(SRC)/styles.css
	ant compile

jar: compile
	ant jar

sstart: jar
	mkdir ../Servidor && cp $(DIST)/* ../Servidor

clean: $(CLASSES)/servidorinterface $(DIST)/* ../Servidor
	rm -r $(CLASSES)/servidorinterface && rm -I $(DIST)/* && rm -r ../Servidor