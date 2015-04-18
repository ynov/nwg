CXX=g++
CXXFLAGS=-O2 -g -Wall -fmessage-length=0 -std=c++11 -I`pwd`/deps/libevent/include
LIBS=-L`pwd` -L`pwd`/deps/libevent/.libs -lnwg -levent
override EXTRALIBS := -lboost_regex

ifeq ($(OS),Windows_NT)
	CXXFLAGS+=-DWIN32 -D_WIN32
	LIBS+=-lws2_32
endif

all: deps.ok libnwg.a tests examples

tests: deps.ok libnwg.a \
	tests/test_nwg_bytebuffer \
	tests/test_nwg_server

examples: deps.ok libnwg.a \
	examples/exm_echoserver \
	examples/exm_httpserverv1

libnwg.a: deps.ok \
	nwg_objectcontainer.o \
	nwg_object.o \
	nwg_bytebuffer.o \
	nwg_session.o \
	nwg_protocolcodec.o \
	nwg_basicprotocolcodec.o \
	nwg_handler.o \
	nwg_server.o \
	nwg_evcb.o
	ar rcs libnwg.a *.o

deps/libevent/.libs/libevent.a:
	cd deps/libevent && \
	./configure --disable-shared && \
	make -j4

deps.ok: \
	deps/libevent/.libs/libevent.a
	touch deps.ok

nwg_objectcontainer.o: nwg_objectcontainer.cc nwg_objectcontainer.h deps.ok
	$(CXX) -c nwg_objectcontainer.cc $(CXXFLAGS)

nwg_object.o: nwg_object.cc nwg_object.h deps.ok
	$(CXX) -c nwg_object.cc $(CXXFLAGS)

nwg_bytebuffer.o: nwg_bytebuffer.cc nwg_bytebuffer.h deps.ok
	$(CXX) -c nwg_bytebuffer.cc $(CXXFLAGS)

nwg_session.o: nwg_session.cc nwg_session.h deps.ok
	$(CXX) -c nwg_session.cc $(CXXFLAGS)

nwg_protocolcodec.o: nwg_protocolcodec.cc nwg_protocolcodec.h deps.ok
	$(CXX) -c nwg_protocolcodec.cc $(CXXFLAGS)

nwg_basicprotocolcodec.o: nwg_basicprotocolcodec.cc nwg_basicprotocolcodec.h deps.ok
	$(CXX) -c nwg_basicprotocolcodec.cc $(CXXFLAGS)

nwg_handler.o: nwg_handler.cc nwg_handler.h deps.ok
	$(CXX) -c nwg_handler.cc $(CXXFLAGS)

nwg_server.o: nwg_server.cc nwg_server.h nwg_common_socket_include.h deps.ok
	$(CXX) -c nwg_server.cc $(CXXFLAGS)

nwg_evcb.o: nwg_evcb.cc nwg_evcb.h nwg_common_socket_include.h deps.ok
	$(CXX) -c nwg_evcb.cc $(CXXFLAGS)

tests/test_nwg_bytebuffer: libnwg.a tests/nwg_bytebuffer_test.cc
	$(CXX) -I`pwd` -o tests/test_nwg_bytebuffer \
		tests/nwg_bytebuffer_test.cc \
		$(CXXFLAGS) $(LIBS)

tests/test_nwg_server: libnwg.a tests/nwg_server_test.cc
	$(CXX) -I`pwd` -o tests/test_nwg_server \
		tests/nwg_server_test.cc \
		$(CXXFLAGS) $(LIBS)

examples/exm_echoserver: libnwg.a examples/echoserver.cc
	$(CXX) -I`pwd` -o examples/exm_echoserver \
		examples/echoserver.cc \
		$(CXXFLAGS) $(LIBS)

examples/exm_httpserverv1: libnwg.a examples/httpserverv1.cc
	$(CXX) -I`pwd` -o examples/exm_httpserverv1 \
		examples/httpserverv1.cc \
		$(CXXFLAGS) $(LIBS)

clean:
	rm -f tests/test_*
	rm -f examples/exm_*
	rm -f *.o
	rm -f *.a
	rm -f deps.ok
