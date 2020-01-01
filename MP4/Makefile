JFLAGS = -g
JAVAC = javac
SOURCE_FILES = \
	./src/Csda.java 
ENTRY_POINT = Csda

vpath %.class bin
vpath %.java src

build: $(SOURCE_FILES:.java=.class)

%.class: %.java
	$(JAVAC) -cp bin -d bin $(JFLAGS) $<

# .SUFFIXES: .java .class
# .java.class:
# 	$(JC) $(JFLAGS) $*.java

# CLASSES = \
# 	./src/Csda.java 

default: new clean classes move
move:
	cp ./src/Csda.class bin
	cp ./src/Node.class bin
	$(RM) ./src/*.class

classes: $(CLASSES:.java=.class)

clean:
	$(RM) ./bin/*.class
new: 
	mkdir -p bin
run:
# 	cd ./bin
	java -cp bin $(ENTRY_POINT) ./input.txt