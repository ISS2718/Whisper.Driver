SRC := ./src/servidorinterface
BUILD := ./build
DIST := ./dist
CLASSES := $(BUILD)/classes

all: jar

compile: $(SRC)/Servidor.java $(SRC)/ServidorInterface.java $(SRC)/ServidorInterfaceDocumentController.java $(SRC)/ServidorInterfaceDocument.fxml $(SRC)/styles.css
	ant compile

jar: compile
	ant jar

start: 
	ant jfxsa-run

clean: $(CLASSES)/servidorinterface $(DIST)/*
	rm -r $(CLASSES)/servidorinterface && rm -I $(DIST)/*