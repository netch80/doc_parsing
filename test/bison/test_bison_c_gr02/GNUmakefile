PROG = prog
OBJS = main.o parser.o lexer.o
CFLAGS = -Wall
CFLAGS-parser.o = -Wno-unused-function
CFLAGS-lexer.o = -Wno-unused-function
LIBS = -lm

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(CFLAGS-$@)

main.o: main.c parser.h lexer.h

parser.c parser.h: parser.y
	bison -o parser.c -d parser.y
#	byacc -o parser.c -p ep_ -d parser.y

lexer.c lexer.h: lexer.l
	flex -o $@ $<

clean:
	rm -f ./*.o
	rm -f prog parser.c parser.h lexer.c lexer.h

.PHONY: clean
