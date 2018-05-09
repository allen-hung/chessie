DESTDIR	=
prefix	= $(DESTDIR)/usr
incdir	= $(prefix)/include/chessie
libdir	= $(prefix)/lib/

SHARED_LIB := libchessie.so

LIB_HEADERS = chessie.h assert.h capturer.h logger.h tester.h
LIB_OBJS = chessie.o capturer.o assert.o logger.o tester.o
CXXFLAGS += -fPIC -Wall -Werror -std=gnu++14

.PHONY: shared_lib example
all: shared_lib example

LIB_HEADERS := $(addprefix chessie/,$(LIB_HEADERS))

install:
	install -d $(libdir) $(incdir)
	install -D --mode=644 $(SHARED_LIB) $(libdir)
	install -D --mode=644 $(LIB_HEADERS) $(incdir)

clean:
	rm -f *.o $(SHARED_LIB)
	make -C examples clean

$(SHARED_LIB): $(LIB_OBJS)
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(LIB_OBJS)

shared_lib: $(SHARED_LIB)

example: shared_lib
	CXXFLAGS="$(CXXFLAGS)" make -C examples
