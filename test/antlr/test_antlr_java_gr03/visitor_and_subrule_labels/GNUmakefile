JAVA ?= java
#JAVA ?= java -cp /home/netch/opt/antlr/antlr-4.5.3-complete.jar
ANTLR ?= java -jar /home/netch/opt/antlr/antlr-4.5.3-complete.jar

run: MyTest.class MyVisitor.class gr03BaseVisitor.class
	$(JAVA) MyTest

%.class: %.java
	javac $<

MyTest.class: gr03BaseVisitor.class
MyVisitor.class: gr03BaseVisitor.class

gr03BaseVisitor.java: gr03.g4
	$(ANTLR) $< -visitor -no-listener

clean:
	rm -f ./*.class ./gr03*.java ./gr03*.tokens

.PHONY: clean
