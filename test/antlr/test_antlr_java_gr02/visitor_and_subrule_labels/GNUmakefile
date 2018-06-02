JAVA ?= java
#JAVA ?= java -cp /home/netch/opt/antlr/antlr-4.5.3-complete.jar
ANTLR ?= java -jar /home/netch/opt/antlr/antlr-4.5.3-complete.jar

run: MyTest.class MyVisitor.class gr02BaseVisitor.class
	$(JAVA) MyTest

%.class: %.java
	javac $<

MyTest.class: gr02BaseVisitor.class
MyVisitor.class: gr02BaseVisitor.class

gr02BaseVisitor.java: gr02.g4
	$(ANTLR) $< -visitor -no-listener

clean:
	rm -f ./*.class ./gr02*.java ./gr02*.tokens

.PHONY: clean
