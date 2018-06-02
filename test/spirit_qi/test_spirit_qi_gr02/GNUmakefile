PROG = prog
OBJS = main.o
CXXFLAGS = -std=gnu++11 -Wall -g -O $(OPTFLAGS)

test: $(PROG)
	./prog

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(OBJS) $(LDFLAGS) $(LIBS)

%.o: %.cc
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(CXXFLAGS-$@)

clean:
	rm -f ./*.o
	rm -f -- $(PROG)

.PHONY: clean test
