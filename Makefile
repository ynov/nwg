CXX=g++
CXXFLAGS=-O2 -g -Wall -fmessage-length=0 -std=c++11
LIBS=-L`pwd` -lnwg -levent
EXTRALIBS=-lboost_regex

all: libnwg.a tests

tests: 	tests/test_nwg_bytebuffer \
	tests/test_nwg_server \
	tests/test_echoserver

libnwg.a: nwg_objectcontainer.o \
	nwg_object.o \
	nwg_bytebuffer.o \
	nwg_session.o \
	nwg_protocolcodec.o \
	nwg_basicprotocolcodec.o \
	nwg_handler.o \
	nwg_server.o \
	nwg_evcb.o
	ar rcs libnwg.a *.o

nwg_objectcontainer.o: nwg_objectcontainer.cc nwg_objectcontainer.h
	$(CXX) -c nwg_objectcontainer.cc $(CXXFLAGS)

nwg_object.o: nwg_object.cc nwg_object.h
	$(CXX) -c nwg_object.cc $(CXXFLAGS)

nwg_bytebuffer.o: nwg_bytebuffer.cc nwg_bytebuffer.h
	$(CXX) -c nwg_bytebuffer.cc $(CXXFLAGS)

nwg_session.o: nwg_session.cc nwg_session.h
	$(CXX) -c nwg_session.cc $(CXXFLAGS)

nwg_protocolcodec.o: nwg_protocolcodec.cc nwg_protocolcodec.h
	$(CXX) -c nwg_protocolcodec.cc $(CXXFLAGS)

nwg_basicprotocolcodec.o: nwg_basicprotocolcodec.cc nwg_basicprotocolcodec.h
	$(CXX) -c nwg_basicprotocolcodec.cc $(CXXFLAGS)

nwg_handler.o: nwg_handler.cc nwg_handler.h
	$(CXX) -c nwg_handler.cc $(CXXFLAGS)

nwg_server.o: nwg_server.cc nwg_server.h nwg_common_socket_include.h
	$(CXX) -c nwg_server.cc $(CXXFLAGS)

nwg_evcb.o: nwg_evcb.cc nwg_evcb.h nwg_common_socket_include.h
	$(CXX) -c nwg_evcb.cc $(CXXFLAGS)
	

tests/test_nwg_bytebuffer: libnwg.a tests/nwg_bytebuffer_test.cc
	$(CXX) -I`pwd` -o tests/test_nwg_bytebuffer \
		tests/nwg_bytebuffer_test.cc \
		$(CXXFLAGS) $(LIBS)

tests/test_nwg_server: libnwg.a tests/nwg_server_test.cc
	$(CXX) -I`pwd` -o tests/test_nwg_server \
		tests/nwg_server_test.cc \
		$(CXXFLAGS) $(LIBS)

tests/test_echoserver: libnwg.a tests/echoserver.cc
	$(CXX) -I`pwd` -o tests/test_echoserver \
		tests/echoserver.cc \
		$(CXXFLAGS) $(LIBS)

clean:
	rm -f tests/test_*
	rm -f *.o
	rm -f *.a
