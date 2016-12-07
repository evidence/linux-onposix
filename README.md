OnPosix
=======

Introduction
------------

OnPosix is a minimal C++ library that allows to speed-up development on
platforms compliant to the POSIX standard (e.g., Linux).

The library offers threading, networking, logging, IPC, etc. Most of the
provided functionalities can be found either inside the [Boost
library](http://www.boost.org) or in a library compliant with the [C++11
standard](http://www.stroustrup.com/C++11FAQ.html).

The library has been developed in the context of the [ERA
project](http://www.era-project.org), financially supported by the European
commission under the FP7 program.

The library is licensed under version 2 of the GNU LIBRARY GENERAL PUBLIC
LICENSE (LGPL).  See file ```COPYING``` for more details.


Installation
------------

To build the library, just type:

	make

The library is built both as shared library (.so) and static library (.a).

To install the library on your system, type

	make install

To install the library in a specific directory, type

	make TARGET_DIR=/absolute/path install

Documentation is created through [Doxygen](http://www.doxygen.org).
To generate the documentation, type

	make doc

Unit testing is done through [Google Test](https://github.com/google/googletest).
To test the library, specify the googletest path by setting the
```GTEST_INCLUDE_DIR``` and ```GTEST_LIB_DIR``` variables inside the
```Makefile```. Then type

	make test
	./test


Examples of usage
-----------------

### Processes

The library provides the class ```onposix::Process``` to run a function or a
program through ```fork()``` and ```fork()+execvp()```, respectively.

Example of usage to run a function:

```cpp
	void function ()
	{
		//...
	}

	int main ()
	{
		Process p(function);
	}
```

Example of usage to run a program - i.e., through ```for()+execvp()```

```cpp
int main ()
{
	std::vector<std::string> args;
	args.push_back("-l");
	args.push_back("*.cpp");
	Process p("ls", args);
}
```


### Threads

The library offers basic mechanisms to create, start and stop threads.  On
Linux, it allows also to set scheduling parameters, thread affinity and signal
handling.

The simplest case to create a thread is to use the ```onposix::SimpleThread``` class:

```cpp
void myfunction (void* arg);

int main ()
{
	int b;
	SimpleThread t (myfunction, (void*) b);
	t.start();
	t.waitForTermination();
}
```

For more complex cases, or if you need a better exchange of arguments (e.g.,
return values), create your own class by inheriting from
```onposix::AbstractThread``` and specifying a ```run()``` method.

Here we provide just a small example. Information about all available methods
can be found in the ```onposix::AbstractThread``` class.

```cpp
class MyThread: public AbstractThread {
	// Put thread arguments here
public:
	MyThread() {
		// Initialize arguments here
	}
	void run() {
		// Put thread code here
		// Access arguments as normal attributes
	}
};

int main ()
{
	MyThread t;
	t.blockSignal(SIGTERM);
	t.start();
	t.waitForTermination();
}
```


### Mutual exclusion

```cpp
PosixMutex m;
m.lock();
//....
m.unlock();
```

### Condition variables

```cpp
PosixCondition c;
PosixMutex m;
c.wait(&m);
//....
c.signal();
```

### File descriptors

```cpp
FileDescriptor fd ("/tmp/myfile", O_RDONLY);
Buffer b (10);
fd.read (&b, b.getSize());
fd.close();
```

It is also possible to use asynchronous read/write operations:

```cpp
void read_handler(Buffer* b, size_t size)
{
	//...
}
FileDescriptor fd ("/tmp/myfile", O_RDONLY);
Buffer b (10);
fd.async_read (read_handler, &b, b.getSize());
fd.close();
```

### Socket descriptors

The library offers mechanisms for both connection-oriented (e.g., TCP) and
connection-less (e.g., UDP) communications. Communications can be either
```AF_UNIX``` or ```AF_INET```.

Here is a small example about a TCP client-server application. More information
can be found in the description of the following classes:

* ```onposix::StreamSocketServer```
* ```onposix::StreamSocketServerDescriptor```
* ```onposix::StreamSocketClientDescriptor```
* ```onposix::DgramSocketClientDescriptor```
* ```onposix::DgramSocketServerDescriptor```


Server-side:

```cpp
void read_handler(Buffer* b, size_t size)
{
	//...
}

int port = 1234;
StreamSocketServer serv (port);
while (true) {
     StreamSocketServerDescriptor des (serv);
     Buffer b (10);
     // Synchronous read:
     des.read(&b, b.getSize());

     //...or asynchronous read:
     fd.async_read (read_handler, &b, b.getSize());
     // ...
}
```

Client-side:

```cpp
int port = 1234;
std::string address = "192.168.10.133";
StreamSocketClientDescriptor des(address, port);
Buffer b (10);
des.write(&b, b.getSize());
```

### Pipes

```cpp
Pipe p;
Buffer b (10);
p.read(&b, b.getSize());
p.close();
```

### FIFOs (AKA "named pipes")

```cpp
FifoDescriptor fd ("/tmp/myfifo", O_RDONLY);
Buffer b (10);
fd.read (&b, b.getSize());
```

### Logging

Log messages can be printed to console and/or to a file with different log
levels.  The following log levels can be set for both ```LOG_LEVEL_CONSOLE```
and ```LOG_LEVEL_FILE```:

* ```LOG_NOLOG```: No logging
* ```LOG_ERRORS```: Only log errors
* ```LOG_WARNINGS```: Log warnings and errors
* ```LOG_ALL```: Log all messages (debug messages included)

These values can be set directly inside the file ```include/Logger.hpp```, or
when including this file as follows:

```cpp
#define LOG_LEVEL_CONSOLE	LOG_WARNINGS
#define LOG_LEVEL_FILE		LOG_ALL
#include "Logger.hpp"
```

To log a message, use

```cpp
LOG_FILE("/tmp/myproject");
DEBUG("hello " << "world");
DEBUG("something " << "strange");
DEBUG("this is an error");
```

### Timing

```cpp
Time t1;
std::cout << t1.getSeconds() << " " << t1.getNSeconds() << std::endl;
```

### Watching multiple descriptors

```cpp
class FileReader: public AbstractDescriptorReader {
	FileDescriptor fd1_;
	FileDescriptor fd2_;
public:
	FileReader(){
		//...
		monitorDescriptor(fd1_);
		monitorDescriptor(fd2_);
	}
	void dataAvailable(PosixDescriptor& des) {
		std::cout << des.getDescriptorNumber() << "ready" << std::endl;
	}
};
```

### Assertions

Assertions provided by this library work also when code is compiled with the
```-NDEBUG``` macro.

Example of usage:

```cpp
int i;
VERIFY_ASSERTION(i == 0);
```

### Much more...

The library also offers:
* Observer designer pattern on descriptors (i.e.,```onposix::DescriptorsMonitor```)
* Buffers (i.e., ```onposix::Buffer```)
* Shared queues (i.e., ```onposix::PosixSharedQueue``` and ```onposix::PosixPrioritySharedQueue```)



