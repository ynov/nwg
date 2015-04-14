CXXFLAGS = -O2 -g -Wall -fmessage-length=0 -std=c++11
LIBS = -levent

all: \
	nwg_objectcontainer.o \
	nwg_object.o \
	nwg_bytebuffer.o \
	nwg_session.o \
	nwg_protocolcodec.o \
	nwg_basicprotocolcodec.o \
	nwg_handler.o \
	nwg_server.o

tests: \
	tests/test_nwg_bytebuffer \
	tests/test_nwg_server \
	tests/test_echoserver

nwg_objectcontainer.o: nwg_objectcontainer.cc nwg_objectcontainer.h
	g++ -c nwg_objectcontainer.cc $(CXXFLAGS)

nwg_object.o: nwg_object.cc nwg_object.h
	g++ -c nwg_object.cc $(CXXFLAGS)

nwg_bytebuffer.o: nwg_bytebuffer.cc nwg_bytebuffer.h
	g++ -c nwg_bytebuffer.cc $(CXXFLAGS)

nwg_session.o: nwg_session.cc nwg_session.h
	g++ -c nwg_session.cc $(CXXFLAGS)

nwg_protocolcodec.o: nwg_protocolcodec.cc nwg_protocolcodec.h
	g++ -c nwg_protocolcodec.cc $(CXXFLAGS)

nwg_basicprotocolcodec.o: nwg_basicprotocolcodec.cc nwg_basicprotocolcodec.h
	g++ -c nwg_basicprotocolcodec.cc $(CXXFLAGS)

nwg_handler.o: nwg_handler.cc nwg_handler.h
	g++ -c nwg_handler.cc $(CXXFLAGS)

nwg_server.o: nwg_server.cc nwg_server.h
	g++ -c nwg_server.cc $(CXXFLAGS)

tests/test_nwg_bytebuffer: \
		tests/nwg_bytebuffer_test.cc \
		nwg_object.o \
		nwg_bytebuffer.o
	g++ -I./ -o tests/test_nwg_bytebuffer \
		tests/nwg_bytebuffer_test.cc \
		nwg_object.o \
		nwg_bytebuffer.o \
		$(CXXFLAGS) $(LIBS)

tests/test_nwg_server: \
		tests/nwg_server_test.cc \
		nwg_objectcontainer.o \
		nwg_object.o \
		nwg_bytebuffer.o \
		nwg_session.o \
		nwg_protocolcodec.o \
		nwg_basicprotocolcodec.o \
		nwg_handler.o \
		nwg_server.o
	g++ -I./ -o tests/test_nwg_server \
		tests/nwg_server_test.cc \
		nwg_objectcontainer.o \
		nwg_object.o \
		nwg_bytebuffer.o \
		nwg_session.o \
		nwg_protocolcodec.o \
		nwg_basicprotocolcodec.o \
		nwg_handler.o \
		nwg_server.o \
		$(CXXFLAGS) $(LIBS)

tests/test_echoserver: \
		tests/echoserver.cc \
		nwg_objectcontainer.o \
		nwg_object.o \
		nwg_bytebuffer.o \
		nwg_session.o \
		nwg_protocolcodec.o \
		nwg_basicprotocolcodec.o \
		nwg_handler.o \
		nwg_server.o
	g++ -I./ -o tests/test_echoserver \
		tests/echoserver.cc \
		nwg_objectcontainer.o \
		nwg_object.o \
		nwg_bytebuffer.o \
		nwg_session.o \
		nwg_protocolcodec.o \
		nwg_basicprotocolcodec.o \
		nwg_handler.o \
		nwg_server.o \
		$(CXXFLAGS) $(LIBS)

clean:
	rm -f tests/test_*
	rm -f *.o
