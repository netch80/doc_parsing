PROG = prog
OBJS = main.o lex.o

CFLAGS = -Og -pipe -std=gnu++11 -Wall -W -g
RE2C ?=	/home/netch/opt/re2c/bin/re2c

test: $(PROG)
	./$(PROG)

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.cc
	$(CXX) -o $@ -c $< $(CFLAGS)

lex.cc: lex.re
	$(RE2C) -o $@ -8 --tags $<

clean:
	rm -f $(PROG) ./*.o ./lex.cc

.PHONY: clean test
