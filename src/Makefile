INCLUDE_DIR = ../include
OBJECTS = Buffer.o DescriptorsMonitor.o FileDescriptor.o FifoDescriptor.o Logger.o  PosixDescriptor.o  StreamSocketServerDescriptor.o DgramSocketServerDescriptor.o StreamSocketServer.o StreamSocketClientDescriptor.o DgramSocketClientDescriptor.o AbstractThread.o PosixMutex.o PosixCondition.o Time.o Pipe.o Process.o
INCLUDES = $(INCLUDE_DIR)/*.hpp
CXXFLAGS += -I$(INCLUDE_DIR) 

all: ../$(LIBNAME).a ../$(LIBNAME).so 

../$(LIBNAME).a: $(OBJECTS)
	ar rcs ../$(LIBNAME).a $(OBJECTS)

../$(LIBNAME).so: $(OBJECTS)
	$(CXX) $(CFLAGS) -shared -Wl,-soname,$(LIBNAME).so -o ../$(LIBNAME).so.$(ONPOSIX_VERSION) $(OBJECTS) -lpthread -lrt
	ln -f -s $(LIBNAME).so.$(ONPOSIX_VERSION) ../$(LIBNAME).so

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

Process.o: $(INCLUDES)

.PHONY: clean

clean:
	-rm -fr *.o ../$(LIBNAME)* 

