CXX=g++
DFLAGS=
CXXINCLUDES=-I`pwd` -I`pwd`/deps/libevent/include -I`pwd`/deps/boost
CXXFLAGS=-O2 -g -Wall -fmessage-length=0 -std=c++11 $(CXXINCLUDES) $(DFLAGS)
LIBDIR=-L`pwd` -L`pwd`/deps/libevent/.libs -L`pwd`/deps/boost/stage/lib
LIBS=$(LIBDIR) -lnwg -levent
BOOST_BOOTSTRAP=./bootstrap.sh
BOOSTLIBS=-lboost_regex -lboost_filesystem -lboost_system

ifeq ($(OS),Windows_NT)
	CXXFLAGS+=-DWIN32 -D_WIN32
	LIBS+=-lws2_32
	BOOST_BOOTSTRAP=cmd /c "bootstrap.bat mingw"
endif

all: libevent \
	libboost \
	libnwg.a \
	tests \
	examples

tests: libevent libnwg.a \
	tests/test_nwg_bytebuffer \
	tests/test_nwg_server

examples: libevent libboost libnwg.a \
	examples/exm_echoserver \
	examples/exm_httpserverv1

libevent_a=deps/libevent/.libs/libevent.a

libnwg.a: $(libevent_a) \
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

$(libevent_a):
	cd deps/libevent && \
	./configure --disable-shared && \
	make -j4

libevent: $(libevent_a)

libboost_regex_a=deps/boost/stage/lib/libboost_regex.a
libboost_filesystem_a=deps/boost/stage/lib/libboost_filesystem.a
libboost_system.a=deps/boost/stage/lib/libboost_system.a
libboost_all=$(libboost_regex_a) $(libboost_filesystem.a) $(libboost_system.a)

$(libboost_all):
	cd deps/boost && \
	$(BOOST_BOOTSTRAP) && \
	./b2 --with-regex --with-filesystem link=static threading=single variant=release toolset=gcc &&\
	cd stage/lib && \
	mv libboost_regex*.a libboost_regex.a && \
	mv libboost_filesystem*.a libboost_filesystem.a && \
	mv libboost_system*.a libboost_system.a

libboost: $(libboost_all)

nwg_objectcontainer.o: nwg_objectcontainer.cc nwg_objectcontainer.h $(libevent_a)
	$(CXX) -c nwg_objectcontainer.cc $(CXXFLAGS)

nwg_object.o: nwg_object.cc nwg_object.h $(libevent_a)
	$(CXX) -c nwg_object.cc $(CXXFLAGS)

nwg_bytebuffer.o: nwg_bytebuffer.cc nwg_bytebuffer.h $(libevent_a)
	$(CXX) -c nwg_bytebuffer.cc $(CXXFLAGS)

nwg_session.o: nwg_session.cc nwg_session.h $(libevent_a)
	$(CXX) -c nwg_session.cc $(CXXFLAGS)

nwg_protocolcodec.o: nwg_protocolcodec.cc nwg_protocolcodec.h $(libevent_a)
	$(CXX) -c nwg_protocolcodec.cc $(CXXFLAGS)

nwg_basicprotocolcodec.o: nwg_basicprotocolcodec.cc nwg_basicprotocolcodec.h $(libevent_a)
	$(CXX) -c nwg_basicprotocolcodec.cc $(CXXFLAGS)

nwg_handler.o: nwg_handler.cc nwg_handler.h $(libevent_a)
	$(CXX) -c nwg_handler.cc $(CXXFLAGS)

nwg_server.o: nwg_server.cc nwg_server.h nwg_common_socket_include.h $(libevent_a)
	$(CXX) -c nwg_server.cc $(CXXFLAGS)

nwg_evcb.o: nwg_evcb.cc nwg_evcb.h nwg_common_socket_include.h $(libevent_a)
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
