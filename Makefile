CXX=g++
TOOLSET=gcc
MAKE=make
DFLAGS=
CXXINCLUDEDIR=-I`pwd` -I`pwd`/deps/libevent/include -I`pwd`/deps/boost
CXXFLAGS=-O2 -g -Wall -fmessage-length=0 -std=c++11 $(CXXINCLUDEDIR) $(DFLAGS)
LIBDIR=-L`pwd`
LIBS=$(LIBDIR) -lnwg -levent
BOOST_BOOTSTRAP=./bootstrap.sh --with-toolset=$(TOOLSET)
BOOSTLIBS=-lboost_regex -lboost_filesystem -lboost_system

ifeq ($(OS),Windows_NT)
	CXXFLAGS+=-DWIN32 -D_WIN32
	LIBS+=-lws2_32
	BOOST_BOOTSTRAP=cmd /c "bootstrap.bat mingw"
endif

all: libnwg.a tests examples

tests: libnwg.a \
	tests/test_nwg_bytebuffer \
	tests/test_nwg_acceptor

examples: libnwg.a \
	examples/exm_echoserver \
	examples/exm_httpserverv1 \
	examples/exm_httpserverv2

libnwg.a: \
	nwg_objectcontainer.o \
	nwg_object.o \
	nwg_bytebuffer.o \
	nwg_session.o \
	nwg_protocolcodec.o \
	nwg_basicprotocolcodec.o \
	nwg_handler.o \
	nwg_acceptor.o \
	nwg_evcb.o
	ar rcs libnwg.a nwg_*.o

##############################################################################
##############################################################################

deps: libboost libevent

### libevent
deps/libevent/.libs/libevent.a:
	cd deps/libevent && \
	./configure --disable-shared && \
	$(MAKE) -j4

libevent.a: deps/libevent/.libs/libevent.a
	cp -f $< $(notdir $<)

libevent: libevent.a

### boost
deps/boost/stage/lib/libboost_regex.a deps/boost/stage/lib/libboost_filesystem.a deps/boost/stage/lib/libboost_system.a:
	cd deps/boost && \
	$(BOOST_BOOTSTRAP) && \
	./b2 --with-regex --with-filesystem link=static threading=single variant=release toolset=$(TOOLSET) && \
	cd stage/lib && \
	if [ ! -e libboost_regex.a ]; then mv -f libboost_regex*.a libboost_regex.a; fi && \
	if [ ! -e libboost_filesystem.a ]; then mv -f libboost_filesystem*.a libboost_filesystem.a; fi && \
	if [ ! -e libboost_system.a ]; then mv -f libboost_system*.a libboost_system.a; fi

libboost_regex.a: deps/boost/stage/lib/libboost_regex.a
	cp -f $< $(notdir $<)

libboost_filesystem.a: deps/boost/stage/lib/libboost_filesystem.a
	cp -f $< $(notdir $<)

libboost_system.a: deps/boost/stage/lib/libboost_system.a
	cp -f $< $(notdir $<)

libboost: libboost_regex.a libboost_filesystem.a libboost_system.a

##############################################################################
##############################################################################

%.o: %.cc %.h nwg_common_socket_include.h
	$(CXX) -c $< $(CXXFLAGS)

examples/exm_%: examples/%.cc libnwg.a
	$(CXX) -o examples/exm_$(notdir $(basename $<)) $< $(CXXFLAGS) $(LIBS) $(BOOSTLIBS)

tests/test_nwg_bytebuffer: libnwg.a tests/nwg_bytebuffer_test.cc
	$(CXX) -o tests/test_nwg_bytebuffer \
		tests/nwg_bytebuffer_test.cc $(CXXFLAGS) $(LIBS)

tests/test_nwg_acceptor: libnwg.a tests/nwg_acceptor_test.cc
	$(CXX) -o tests/test_nwg_acceptor \
		tests/nwg_acceptor_test.cc $(CXXFLAGS) $(LIBS)

clean:
	rm -f tests/test_*
	rm -f examples/exm_*
	rm -f nwg_*.o
	rm -f libnwg.a
