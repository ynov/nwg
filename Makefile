CXX=g++
TOOLSET=gcc
MAKE=make
DFLAGS=
CXXINCLUDEDIR=-I`pwd`/src -I`pwd`/deps/libevent/include -I`pwd`/deps/boost
CXXFLAGS=-O2 -g -Wall -fmessage-length=0 -std=c++11 $(CXXINCLUDEDIR) $(DFLAGS)
LIBDIR=-L`pwd`/lib
LIBS=$(LIBDIR) -lnwg -levent
BOOST_BOOTSTRAP=./bootstrap.sh --with-toolset=$(TOOLSET)
BOOSTLIBS=-lboost_regex -lboost_filesystem -lboost_system

ifeq ($(OS),Windows_NT)
	CXXFLAGS+=-DWIN32 -D_WIN32
	LIBS+=-lws2_32
	BOOST_BOOTSTRAP=cmd /c "bootstrap.bat mingw"
endif

all: lib/libnwg.a tests examples

tests: lib/libnwg.a \
	tests/test_nwg_bytebuffer \
	tests/test_nwg_acceptor

examples: lib/libnwg.a \
	examples/exm_echoserver \
	examples/exm_echoclient \
	examples/exm_httpserverv1 \
	examples/exm_httpserverv2

lib/libnwg.a: \
	nwg_objectcontainer.o \
	nwg_object.o \
	nwg_bytebuffer.o \
	nwg_session.o \
	nwg_protocolcodec.o \
	nwg_basicprotocolcodec.o \
	nwg_handler.o \
	nwg_eventloop.o \
	nwg_service.o \
	nwg_acceptor.o \
	nwg_connector.o \
	nwg_evcb.o
	ar rcs lib/libnwg.a nwg_*.o

##############################################################################
##############################################################################

deps: libboost libevent

### libevent
deps/libevent/.libs/libevent.a:
	cd deps/libevent && \
	./configure --disable-shared && \
	$(MAKE) -j4

lib/libevent.a: deps/libevent/.libs/libevent.a
	cp -f $< lib/$(notdir $<)

libevent: lib/libevent.a

### boost
deps/boost/stage/lib/libboost_regex.a deps/boost/stage/lib/libboost_filesystem.a deps/boost/stage/lib/libboost_system.a:
	cd deps/boost && \
	$(BOOST_BOOTSTRAP) && \
	./b2 --with-regex --with-filesystem link=static threading=single variant=release toolset=$(TOOLSET) && \
	cd stage/lib && \
	if [ ! -e libboost_regex.a ]; then mv -f libboost_regex*.a libboost_regex.a; fi && \
	if [ ! -e libboost_filesystem.a ]; then mv -f libboost_filesystem*.a libboost_filesystem.a; fi && \
	if [ ! -e libboost_system.a ]; then mv -f libboost_system*.a libboost_system.a; fi

lib/libboost_regex.a: deps/boost/stage/lib/libboost_regex.a
	cp -f $< lib/$(notdir $<)

lib/libboost_filesystem.a: deps/boost/stage/lib/libboost_filesystem.a
	cp -f $< lib/$(notdir $<)

lib/libboost_system.a: deps/boost/stage/lib/libboost_system.a
	cp -f $< lib/$(notdir $<)

libboost: lib/libboost_regex.a lib/libboost_filesystem.a lib/libboost_system.a

##############################################################################
##############################################################################

%.o: src/%.cc src/%.h src/nwg_common_socket_include.h
	$(CXX) -c $< $(CXXFLAGS)

examples/exm_%: examples/%.cc lib/libnwg.a
	$(CXX) -o examples/exm_$(notdir $(basename $<)) $< $(CXXFLAGS) $(LIBS) $(BOOSTLIBS)

tests/test_nwg_bytebuffer: tests/nwg_bytebuffer_test.cc lib/libnwg.a
	$(CXX) -o tests/test_nwg_bytebuffer \
		tests/nwg_bytebuffer_test.cc $(CXXFLAGS) $(LIBS)

tests/test_nwg_acceptor: tests/nwg_acceptor_test.cc lib/libnwg.a
	$(CXX) -o tests/test_nwg_acceptor \
		tests/nwg_acceptor_test.cc $(CXXFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f tests/test_*
	rm -f examples/exm_*
	rm -f nwg_*.o
	rm -f lib/libnwg.a
