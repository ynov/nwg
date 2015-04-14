CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -std=c++11

tests: tests/test_nwg_bytebuffer

all: nwg_object.o nwg_bytebuffer.o

nwg_object.o: nwg_object.cc nwg_object.h
	g++ -c nwg_object.cc $(CXXFLAGS)

nwg_bytebuffer.o: nwg_bytebuffer.cc nwg_bytebuffer.h
	g++ -c nwg_bytebuffer.cc $(CXXFLAGS)

tests/test_nwg_bytebuffer: tests/nwg_bytebuffer_test.cc nwg_object.o nwg_bytebuffer.o
	g++ -I./ -o tests/test_nwg_bytebuffer \
		tests/nwg_bytebuffer_test.cc \
		nwg_object.o \
		nwg_bytebuffer.o \
		$(CXXFLAGS)

clean:
	rm -f *.o
