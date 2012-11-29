LIBNAME = libonposix
INCLUDE_DIR = ../include
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra -fPIC -I$(INCLUDE_DIR) 
OBJECTS = Buffer.o DescriptorsMonitor.o FileDescriptor.o FifoDescriptor.o Logger.o  PosixDescriptor.o  StreamSocketServerDescriptor.o DgramSocketServerDescriptor.o StreamSocketServer.o StreamSocketClientDescriptor.o DgramSocketClientDescriptor.o AbstractThread.o PosixMutex.o PosixCondition.o Time.o Pipe.o
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

Buffer.o: $(INCLUDES)

DescriptorsMonitor.o: $(INCLUDES)

FileDescriptor.o: $(INCLUDES)

FifoDescriptor.o: $(INCLUDES)

Logger.o: $(INCLUDES)

PosixDescriptor.o: $(INCLUDES)

AbstractThread.o: $(INCLUDES)

PosixMutex.o: $(INCLUDES)

PosixCondition.o: $(INCLUDES)

StreamSocketServerDescriptor.o: $(INCLUDES)

StreamSocketServer.o: $(INCLUDES)

DgramSocketServerDescriptor.o: $(INCLUDES)

StreamSocketClientDescriptor.o: $(INCLUDES)

DgramSocketClientDescriptor.o: $(INCLUDES)

Time.o: $(INCLUDES)

Pipe.o: $(INCLUDES)

.PHONY: clean

clean:
	-rm -fr *.o ../$(LIBNAME)* ../test
