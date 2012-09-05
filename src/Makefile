LIBNAME = libonposix
INCLUDE_DIR = ../include
CXX = g++
CXXFLAGS = -O3 -Wall -fPIC -I$(INCLUDE_DIR) 
OBJECTS = Buffer.o DescriptorsMonitor.o FileDescriptor.o FifoDescriptor.o Logger.o  PosixDescriptor.o  TcpSocketServerDescriptor.o UdpSocketServerDescriptor.o TcpSocketServer.o TcpSocketClientDescriptor.o UdpSocketClientDescriptor.o AbstractThread.o PosixMutex.o PosixCondition.o Time.o Pipe.o
INCLUDES = $(INCLUDE_DIR)/*.hpp

all: ../$(LIBNAME).a ../$(LIBNAME).so ../test

../$(LIBNAME).a: $(OBJECTS)
	ar rcs ../$(LIBNAME).a $(OBJECTS)

../$(LIBNAME).so: $(OBJECTS)
	$(CXX) $(CFLAGS) -shared -Wl,-soname,$(LIBNAME).so.1 -o ../$(LIBNAME).$(ONPOSIX_VERSION) $(OBJECTS) -lpthread
	ln -f -s $(LIBNAME).$(ONPOSIX_VERSION) ../$(LIBNAME).so

../test: test.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o ../test test.o $(OBJECTS) -lpthread

test.o: $(INCLUDES) test.cpp
	$(CXX) $(CXXFLAGS) -c test.cpp

Buffer.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c Buffer.cpp

DescriptorsMonitor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c DescriptorsMonitor.cpp

FileDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c FileDescriptor.cpp

FifoDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c FifoDescriptor.cpp

Logger.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c Logger.cpp

PosixDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c PosixDescriptor.cpp

AbstractThread.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c AbstractThread.cpp

PosixMutex.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c PosixMutex.cpp

PosixCondition.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c PosixCondition.cpp

TcpSocketServerDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c TcpSocketServerDescriptor.cpp

TcpSocketServer.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c TcpSocketServer.cpp

UdpSocketServerDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c UdpSocketServerDescriptor.cpp

TcpSocketClientDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c TcpSocketClientDescriptor.cpp

UdpSocketClientDescriptor.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c UdpSocketClientDescriptor.cpp

Time.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c Time.cpp

Pipe.o: $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c Pipe.cpp

.PHONY: clean

clean:
	-rm -fr *.o ../$(LIBNAME)* ../test
