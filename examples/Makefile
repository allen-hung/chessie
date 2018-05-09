EXAMPLE := example

OBJS = main.o testcases.o

CXXFLAGS += -I.. -Wno-unused-variable

all: $(EXAMPLE)

clean:
	rm -f *.o $(EXAMPLE)

$(EXAMPLE): $(OBJS)
	$(CXX) -o $@ $(OBJS) -L.. -lchessie
