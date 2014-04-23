/*
 * test.cpp
 *
 * Copyright (C) 2012 Evidence Srl - www.evidence.eu.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "gtest/gtest.h"

#include <cstdio>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>


#include "Buffer.hpp"
#include "AbstractDescriptorReader.hpp"
#include "DescriptorsMonitor.hpp"
#include "FileDescriptor.hpp"
#include "FifoDescriptor.hpp"
#include "Logger.hpp"
#include "StreamSocketServerDescriptor.hpp"
#include "StreamSocketServer.hpp"
#include "StreamSocketClientDescriptor.hpp"
#include "AbstractThread.hpp"
#include "Time.hpp"
#include "SimpleThread.hpp"
#include "Process.hpp"
#include "Pipe.hpp"


// Uncomment to enable Linux-specific methods:
// #define ONPOSIX_LINUX_SPECIFIC


using namespace onposix;

// ======================================================================
//   LOGGER
// ======================================================================


class LoggerTest: public ::testing::Test {
public:
	~LoggerTest(){
		// Disable further printing
		LOG_FILE("/tmp/test-logger");
	}
};




// ======================================================================
//   BUFFER
// ======================================================================

TEST (BufferTest, Size0)
{
	// Buffer with size 0
	bool catched = false;
	try {
		Buffer b1(0);
	} catch (...) {
		catched = true;
	}
	ASSERT_TRUE(catched)
		<< "ERROR: Exception of 0-size buffer not thrown!";
}

TEST (BufferTest, Size0New)
{
	// Buffer with size 0 (with new)
	bool catched = false;
	try {
		new Buffer (0);
	} catch (...) {
		catched = true;
	}
	ASSERT_TRUE(catched)
		<< "ERROR: Exception of 0-size buffer wth new not thrown!";
}


TEST (BufferTest, Size)
{
	// Right size;
	bool catched = false;
	try {
		Buffer b3(100);
		ASSERT_TRUE(b3.getSize() == 100)
			<< "ERROR: getSize not working";
		ASSERT_TRUE(b3.getBuffer() != NULL)
			<< "ERROR: getBuffer not working";
		b3[0] = 'c';
		ASSERT_TRUE(b3[0] == 'c')
			<< "ERROR: first byte";
		b3[1] = 'p';
		ASSERT_TRUE(b3[1] == 'p')
			<< "ERROR: second byte";
		b3[99] = 'd';
		ASSERT_TRUE(b3[99] == 'd')
			<< "ERROR: last byte";
		const char* s = "buffer test";
		b3.fill (s, 11);
	} catch (...) {
		catched = true;
	}
	ASSERT_FALSE(catched)
		<< "ERROR: exeption thrown";
}

TEST (BufferTest, OutOfBoundaryChar)
{
	// Out of boundary
	bool catched = false;
	try {
		Buffer b4 (100);
		ASSERT_TRUE(b4.getBuffer() != NULL)
			<< "ERROR: Null-pointer returned!";
		b4[100] = 'd';
	} catch (...) {
		catched = true;
	}
	ASSERT_TRUE(catched)
		<< "ERROR: exception not thrown about exceeding buffer size";
}

TEST (BufferTest, RightSize)
{
	// Right size (with new);
	bool catched = false;
	try {
		Buffer* b5 = new Buffer (1000);
		ASSERT_TRUE(b5->getSize() == 1000)
			<< "ERROR: getSize() returned a wrong size";
		ASSERT_TRUE(b5->getBuffer() != NULL)
			<< "ERROR: Null-pointer returned!";
		delete b5;
	} catch (...) {
		catched = true;
	}
	ASSERT_FALSE(catched)
		<< "ERROR: exception thrown during normal usage of buffer";
}


TEST (BufferTest, OutOfBoundaryString)
{
	// Out of boundary
	bool catched = false;
	try {
		Buffer b6 (5);
		ASSERT_TRUE(b6.getBuffer() != NULL)
			<< "ERROR: Null-pointer returned!";
		const char* s = "buffer test";
		b6.fill (s, 11);
	} catch (...) {
		catched = true;
	}
	ASSERT_TRUE(catched == true)
		<< "ERROR: exception not twhrown when going over the buffer limit";
}

TEST (BufferTest, Comparison)
{
	// Compare
	bool catched = false;
	try {
		Buffer b7 (50);
		const char* s1 = "ABCDEFGHILMNOPQRSTUVZ";
		const char* s2 = "ABCDEFGHILMNOPQRSTUVZ";
		b7.fill(s1, 21);
		Buffer b8 (100);
		b8.fill(s2, 21);
		ASSERT_TRUE(b8.compare(&b7, 21))
			<< "ERROR: 22nd byte of Buffer is different";
		const char* s3 = "BCDEFGHIL";
		b8.fill (s3, 5);
		ASSERT_FALSE(b8.compare(&b7, 21))
			<< "ERROR: 22nd byte of Buffer has not been rewritten";
		b8.fill(&b7, 21);
		ASSERT_TRUE(b8.compare(&b7, 21))
			<< "ERROR: 22nd byte of Buffer is different (2nd time)";
	} catch (...) {
		catched = true;
	}
	ASSERT_FALSE(catched)
		<< "ERROR: exception thworn for normal comparison of buffers";
}


// ======================================================================
//   FIFOs
// ======================================================================


TEST (FifoDescriptorTest, MainTest)
{
	unlink("/tmp/test-fifo-1");
	FifoDescriptor fd("/tmp/test-fifo-1", O_WRONLY|O_CREAT, S_IRWXU);
	std::cout << "\t\tFifo size: " << fd.getCapacity() << std::endl;
}

bool read_fifo_handler_called = false;

void read_fifo_handler(Buffer* b, size_t size)
{
	read_fifo_handler_called = true;
	ASSERT_TRUE (size == 15)
	    << "ERROR: read the wrong number of bytes!";
	ASSERT_TRUE(b->compare("ABC", 3))
	   << "ERROR: content of buffer wrong";
}

void reader_fifo(void*)
{
	// Reader
	Buffer b2(20);
	ASSERT_TRUE(read_fifo_handler_called == false)
	   << "ERROR: initial value of read_fifo_handler_called";
	FifoDescriptor fd2("/tmp/test-async-1", O_RDONLY);
	fd2.async_read(read_fifo_handler, &b2, 20);
	ASSERT_TRUE(read_fifo_handler_called == false)
	   << "ERROR: value of read_fifo_handler_called modified";
	sleep(5);
}


TEST (FifoDescriptorTest, AsyncRead)
{
	// Writer:
	unlink("/tmp/test-async-1");
	FifoDescriptor fd1("/tmp/test-async-1", O_RDWR|O_CREAT, S_IRWXU);
	const char* s1 = "ABCDEFGHILMNOPQ";
	Buffer b1 (15);
	b1.fill (s1, 15);

	SimpleThread t (reader_fifo, 0);
	t.start();

	// Again writer
	fd1.write(&b1, 15);
	sleep(10);
	
	// Again reader
	ASSERT_TRUE(read_fifo_handler_called == true)
	   << "ERROR: handler not called";
}

// ======================================================================
//   FILEs
// ======================================================================


TEST (FileDescriptorTest, WriteWhenNotExisting)
{
	unlink("/tmp/test-file-1");

	bool catched = false;
	try {
		FileDescriptor fd1("/tmp/test-file-1", O_WRONLY);
	} catch (...) {
		catched = true;
	}
	ASSERT_TRUE(catched)
		<< "ERROR: exception not thrown for write only on not existing file";
}

TEST (FileDescriptorTest, CreateUsage)
{
	bool catched = false;
	try {
		FileDescriptor fd2("/tmp/test-file-1", O_WRONLY|O_CREAT,
		    S_IRWXU);
		const char* s1 = "ABCDEFGHIL";
		Buffer b1(10);
		b1.fill(s1, 10);
		fd2.write(&b1, 10);
		fd2.close();
	} catch (...) {
		catched = true;
	}
	ASSERT_FALSE(catched)
		<< "ERROR: exception thrown when using O_CREAT";
}

TEST (FileDescriptorTest, AppendUsage)
{
	bool catched = false;
	try {
		FileDescriptor fd3("/tmp/test-file-1", O_WRONLY|O_APPEND);
		const char* s2 = "MNOPQRSTUVZ";
		fd3.write(s2, 11);
		fd3.close();
	} catch (...) {
		catched = true;
	}
	ASSERT_FALSE(catched)
		<< "ERROR: exception thrown when using O_APPEND";
}

TEST (FileDescriptorTest, ReadOnly)
{
	bool catched = false;
	try {
		FileDescriptor fd4("/tmp/test-file-1", O_RDONLY);
		Buffer b(21);
		fd4.read(&b, 21);
		const char* s3 ="ABCDEFGHILMNOPQRSTUVZ";
		ASSERT_TRUE(b.compare(s3, 21));
		FileDescriptor fd5 = fd4;
		FileDescriptor fd6 ("/tmp/test", O_RDONLY|O_CREAT, S_IRWXU);
		fd6 = fd5;
		fd4.close();
	} catch (...) {
		catched = true;
	}
	ASSERT_FALSE(catched)
		<< "ERROR: exception thrown when reading a file";
}



// ======================================================================
//   PROCESSES
// ======================================================================

Process * proc1 = 0;
bool process_bool1 = false;

void process1 ()
{
	ASSERT_TRUE(proc1->getPid() == getpid())
		<< "ERROR: pid for child wrong";
	process_bool1 = true;
}

TEST (ProcessTest, exec)
{
	std::vector<std::string> args;
	args.push_back("-l");
	args.push_back("*.cpp");
	Process proc0("ls", args);

	proc1 = new Process(process1);
	sleep(3);
	ASSERT_TRUE(proc1->getPid() != getpid())
		<< "ERROR: pid for parent wrong";
	ASSERT_TRUE(process_bool1 == false)
		<< "ERROR: process_bool1 changed value";
	proc1->sendSignal(SIGKILL);
}

Pipe process_pipe;

void process2()
{
	Buffer b(10);
	b.fill("XYZWA", 5);
	process_pipe.write(&b, 5);

}

TEST (ProcessTest, processPipe)
{
	Process p (process2);
	sleep(5);
	Buffer b(10);
	process_pipe.read(&b, 5);
	ASSERT_TRUE(b.compare("XYZWA", 5))
		<< "ERROR: Different values in buffer!";
	p.sendSignal(SIGKILL);
}

// ======================================================================
//   THREADS
// ======================================================================

class MyThread: public AbstractThread {
public:
	bool invoked_;

        void run() {
		invoked_ = true;
		sleep(5);
	}
 };


TEST (ThreadTest, StartStop)
{
	MyThread t;
	t.invoked_ = false;
	ASSERT_TRUE(t.start())
		<< "ERROR: can't start the thread!";
	sleep(2);
	ASSERT_TRUE(t.invoked_)
		<< "ERROR: start() not invoked";
	ASSERT_TRUE(t.stop())
		<< "ERROR: can't stop the thread!";
}


#if defined(ONPOSIX_LINUX_SPECIFIC) && defined(__GLIBC__) && \
    ((__GLIBC__ > 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ > 3)))
TEST (ThreadTest, Affinity)
{
	MyThread t;
	ASSERT_TRUE(t.start())
		<< "ERROR: can't start the thread!";
	std::vector<bool> g(2);
	t.getAffinity(&g);
	
	g[0] = true;
	g[1] = false;
	t.setAffinity(g);
	t.getAffinity(&g);
	ASSERT_TRUE(g[0])
		<< "ERROR: Affinity proc0 not properly set";
	ASSERT_FALSE(g[1])
		<< "ERROR: Affinity proc1 not properly set";
	ASSERT_TRUE(t.stop())
		<< "ERROR: can't stop the thread!";
}
#endif /* ONPOSIX_LINUX_SPECIFIC && GLIBC */

TEST (ThreadTest, SchedParam)
{
	MyThread t;
	ASSERT_TRUE(t.start())
		<< "ERROR: can't start the thread!";
	int policy, prio;
	t.getSchedParam(&policy, &prio);

	policy = SCHED_RR;
	prio = 1;
	ASSERT_TRUE(t.setSchedParam(policy, prio))
		<< "ERROR in setSchedParam!";

	t.getSchedParam(&policy, &prio);
	ASSERT_TRUE(policy == SCHED_RR)
		<< "ERROR: scheduling policy not set (this test needs to be run as root)";
	ASSERT_TRUE(prio == 1)
		<< "ERROR: scheduling priority not set (this test needs to be run as root)";
	ASSERT_TRUE(t.stop())
		<< "ERROR: can't stop the thread!";
}

int value = 0;

void change_value (void* arg)
{
	(*((int*) arg))++;
}

TEST (ThreadTest, SimpleThread)
{
	ASSERT_TRUE(value == 0)
	    << "ERROR: Initial value of variable is not false";
	SimpleThread t (change_value, (void*) &value);
	sleep(2);
	ASSERT_TRUE(value == 0)
	    << "ERROR: value of variable modified";
	t.start();
	t.waitForTermination();
	ASSERT_TRUE(value == 1)
	    << "ERROR: value of variable not incremented";
	t.start();
	t.waitForTermination();
	ASSERT_TRUE(value == 2)
	    << "ERROR: value of variable not incremented (2nd time)";
}



// ======================================================================
//   SOCKETS
// ======================================================================

class MyThreadSock: public AbstractThread {
        // Put thread arguments here
	int sock;
	struct sockaddr_un serv_addr;
 public:
	MyThreadSock(): sock(-1){
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, "/tmp/test-socket");
	}
        void run() {
		sleep(5);
		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		const char* b1 = "ABCDEFGHILMNOP";
		ASSERT_EQ(write (sock, b1, 10), 10)
			<< "ERROR writing data to socket.";
		sleep(2);
		const char* b2 = "XYZ";
		ASSERT_EQ(write (sock, b2, 3), 3)
			<< "ERROR writing data to socket (2)";
        }
 };


class SocketReader: public AbstractDescriptorReader {
	StreamSocketServerDescriptor* s_;
 public:
        SocketReader(DescriptorsMonitor& dm, StreamSocketServerDescriptor* s):
		AbstractDescriptorReader(dm), s_(s){
                                monitorDescriptor(*s);
        }
        ~SocketReader(){}
        virtual void dataAvailable(PosixDescriptor& descriptor) {
                Buffer buff(3);
                s_->read(&buff, buff.getSize());
                ASSERT_EQ(descriptor.getDescriptorNumber(), 
		    s_->getDescriptorNumber())
			<< "ERROR: dataAvailable called with wrong descriptor!";

		ASSERT_TRUE(buff.compare("XYZ", 3))
			<< "ERROR in Socket";
	}
 };


TEST (ThreadSockTest, MainTest)
{
	unlink("/tmp/test-socket");
	StreamSocketServer serv("/tmp/test-socket");
	MyThreadSock t;
	t.start();

	StreamSocketServerDescriptor des (serv);
	Buffer b (10);
	des.read(&b, b.getSize());
	ASSERT_TRUE(b.compare("ABCDEFGHIL", 10))
		<< "Error in Socket (2)";
	DescriptorsMonitor dm;
	SocketReader sr(dm, &des);
	dm.wait();
}


bool read_socket_handler_called = false;

void read_socket_handler(Buffer* b, size_t size)
{
	read_socket_handler_called = true;
	ASSERT_TRUE (size == 15)
	    << "ERROR: read the wrong number of bytes!";
	ASSERT_TRUE(b->compare("ABC", 3))
	   << "ERROR: content of buffer wrong";
}

void async_socket_reader(void*)
{
	StreamSocketClientDescriptor sk2("/tmp/test-async-socket");

	// Reader
	Buffer b2(20);
	ASSERT_TRUE(read_socket_handler_called == false)
	   << "ERROR: initial value of read_socket_handler_called";
	sk2.async_read(read_socket_handler, &b2, 15);
	ASSERT_TRUE(read_socket_handler_called == false)
	   << "ERROR: value of read_socket_handler_called modified";
	sleep(10);
}


TEST (ThreadSockAsyncTest, AsyncRead)
{
	// Writer:
	unlink("/tmp/test-async-socket");
	StreamSocketServer des("/tmp/test-async-socket");

	SimpleThread t (async_socket_reader, 0);
	t.start();

	sleep(2);

	StreamSocketServerDescriptor sk1(des);
	const char* s1 = "ABCDEFGHILMNOPQ";
	Buffer b1 (15);
	b1.fill (s1, 15);

	sleep(4);

	// Again writer
	sk1.write(&b1, 15);
	sk1.flush();
	sleep(10);
	
	// Again reader
	ASSERT_TRUE(read_socket_handler_called == true)
	   << "ERROR: handler not called";

	t.waitForTermination();
}



// ======================================================================
//   TIME 
// ======================================================================

TEST (TimeTest, MainTest)
{
	Time a;
	Time b;
	ASSERT_TRUE((a < b) || (a == b))
		<< "ERROR: second time higher than first time!";
}


TEST (TimeTest, ResetTime)
{
	Time a;
	Time b;
	a.resetToCurrentTime();	
	ASSERT_TRUE(a > b)
		<< "ERROR: Timer a not resetted to current time";
}


TEST (TimeTest, OperatorEqEq)
{
	Time a;
	Time b;
	ASSERT_TRUE((a ==a) && (b == b))
		<< "ERROR: in operator== for class Time";
}



int main(int argc, char **argv)
{
	DEBUG("test debug 1");
	WARNING("test warning 1");
	ERROR("test error 1");
	LOG_FILE("/tmp/test.cloud");
	DEBUG("test debug 2");
	WARNING("test warning 2");
	ERROR("test error 2");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
