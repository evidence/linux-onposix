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

#include "Buffer.hpp"
#include "AbstractDescriptorReader.hpp"
#include "DescriptorsMonitor.hpp"
#include "FileDescriptor.hpp"
#include "FifoDescriptor.hpp"
#include "Logger.hpp"
#include "StreamSocketServerDescriptor.hpp"
#include "StreamSocketServer.hpp"
#include "AbstractThread.hpp"
#include "Time.hpp"

#include "gtest/gtest.h"

#include <cstdio>
#include <cassert>
#include <iostream>

// #define ONPOSIX_LINUX_SPECIFIC


using namespace onposix;

// ======================================================================
//   LOGGER
// ======================================================================


class LoggerTest: public ::testing::Test {
public:
	~LoggerTest(){
		// Disable further printing
		DEBUG_CONF("/tmp/test-logger", OFF, OFF);
	}
};

TEST_F (LoggerTest, NoPrintConfigured)
{
	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", OFF, OFF);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 7";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 8";

	DEBUG(WARN, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 9";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 10";

	DEBUG(ERROR, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 11";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 12";

	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 13";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 14";


	DEBUG_CONF("/tmp/test-logger", ERROR, OFF);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 15";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 16";
	
	DEBUG(WARN, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 17";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 18";
	
	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 19";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 20";

	DEBUG_CONF("/tmp/test-logger", WARN, OFF);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 21";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 22";
	
	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 23";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 24";

	DEBUG_CONF("/tmp/test-logger", OFF, ERROR);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 25";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 26";

	DEBUG(WARN, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 27";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 28";
	
	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 29";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 30";

	DEBUG_CONF("/tmp/test-logger", OFF, WARN);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 31";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 32";

	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 33";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 34";

	DEBUG_CONF("/tmp/test-logger", ERROR, ERROR);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 35";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 36";

	DEBUG(WARN, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 37";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 38";
	
	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 39";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 40";

	DEBUG_CONF("/tmp/test-logger", WARN, WARN);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 41";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 42";

	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 43";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 44";
}



TEST_F (LoggerTest, OnlyFilePrint)
{
	// PRINT ONLY ON FILE
	DEBUG_CONF("/tmp/test-logger", DEBUG, OFF);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 45";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 46";

	DEBUG(WARN, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 47";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 48";

	DEBUG(ERROR, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 49";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 50";

	DEBUG(DEBUG, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 51";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 52";

	DEBUG_CONF("/tmp/test-logger", WARN, OFF);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 53";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 54";
	
	DEBUG(WARN, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 55";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 56";
	
	DEBUG(ERROR, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 57";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 58";

	DEBUG_CONF("/tmp/test-logger", ERROR, OFF);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 59";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 60";

	DEBUG(ERROR, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 61";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 62";
}


TEST_F (LoggerTest, OnlyScreenPrint)
{
	// PRINT ONLY ON SCREEN
	DEBUG_CONF("/tmp/test-logger", OFF, DEBUG);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 63";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 64";

	DEBUG(WARN, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 65";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 66";

	DEBUG(ERROR, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 67";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 68";

	DEBUG(DEBUG, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 69";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 70";

	DEBUG_CONF("/tmp/test-logger", OFF, WARN);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 71";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 72";
	
	DEBUG(WARN, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 73";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 74";
	
	DEBUG(ERROR, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 75";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 76";

	DEBUG_CONF("/tmp/test-logger", OFF, ERROR);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 77";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 78";
	
	DEBUG(ERROR, "Message");
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 79";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 80";
}

TEST_F (LoggerTest, ScreenAndFilePrint)
{
	// PRINT ON SCREEN AND FILE
	DEBUG_CONF("/tmp/test-logger", DEBUG, DEBUG);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 81";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 82";
	
	DEBUG(WARN, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 83";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 84";
	
	DEBUG(ERROR, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 85";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 86";
	
	DEBUG(DEBUG, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 87";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 88";

	DEBUG_CONF("/tmp/test-logger", WARN, WARN);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 89";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 90";
	
	DEBUG(WARN, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 91";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 92";
	
	DEBUG(ERROR, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 93";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 94";

	DEBUG_CONF("/tmp/test-logger", ERROR, DEBUG);
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 95";
	ASSERT_FALSE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 96";
	
	DEBUG(ERROR, "Message");
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnFile())
		<< "Logger: error 97";
	ASSERT_TRUE(Logger::getInstance().latestMsgPrintedOnScreen())
		<< "Logger: error 98";
}



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


#ifdef ONPOSIX_LINUX_SPECIFIC
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
#endif /* ONPOSIX_LINUX_SPECIFIC */

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

		ASSERT_TRUE(!strncmp(buff.getBuffer(), "XYZ", 3))
			<< "ERROR in Socket";
	}
 };


TEST (ThreadSockTest, MainTest) {
	unlink("/tmp/test-socket");
	StreamSocketServer serv("/tmp/test-socket");
	MyThreadSock t;
	t.start();

	StreamSocketServerDescriptor des (serv);
	Buffer b (10);
	des.read(&b, b.getSize());
	ASSERT_TRUE(!strncmp(b.getBuffer(), "ABCDEFGHIL", 10))
		<< "Error in Socket (2)";
	DescriptorsMonitor dm;
	SocketReader sr(dm, &des);
	dm.wait();
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
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
