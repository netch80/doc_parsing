JAVA ?= java
#JAVA ?= java -cp /home/netch/opt/antlr/antlr-4.5.3-complete.jar
ANTLR ?= java -jar /home/netch/opt/antlr/antlr-4.5.3-complete.jar

run: MyTest.class MyVisitor.class gr04BaseVisitor.class
	$(JAVA) MyTest

%.class: %.java
	javac $<

MyTest.class: gr04BaseVisitor.class
MyVisitor.class: gr04BaseVisitor.class ScalarSV.class IdentifierSV.class IndexedReferenceSV.class

gr04BaseVisitor.java: gr04.g4
	$(ANTLR) $< -visitor -no-listener

clean:
	rm -f ./*.class ./gr04*.java ./gr04*.tokens

.PHONY: clean
