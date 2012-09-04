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
#include "SocketServerDescriptor.hpp"
#include "SocketTcpServer.hpp"
#include "AbstractThread.hpp"
#include "Time.hpp"

#include <cstdio>
#include <cassert>
#include <iostream>

using namespace onposix;

#define EXPECT(file, screen) {\
	assert(Logger::getInstance().latestMsgPrintedOnFile() == file);\
	assert(Logger::getInstance().latestMsgPrintedOnScreen() == screen);\
}

void testBuffer()
{
	// Buffer with size 0
	bool catched = false;
	try {
		Buffer b1(0);
	} catch (...) {
		std::cerr << "1. OK: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == true);

	// Buffer with size 0 (with new)
	catched = false;
	try {
		new Buffer (0);
	} catch (...) {
		std::cerr << "2. OK: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == true);

	// Right size;
	catched = false;
	try {
		Buffer b3 (100);
		assert(b3.getSize() == 100);
		assert(b3.getBuffer() != NULL);
		b3[0] = 'c';
		assert (b3[0] == 'c');
		b3[1] = 'p';
		assert (b3[1] == 'p');
		b3[99] = 'd';
		assert (b3[99] == 'd');
		const char* s = "buffer test";
		b3.fill (s, 11);
	} catch (...) {
		std::cerr << "4. ERROR: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == false);

	// Out of boundary
	catched = false;
	try {
		Buffer b4 (100);
		assert(b4.getBuffer() != NULL);
		b4[100] = 'd';
	} catch (...) {
		std::cerr << "5. OK: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == true);

	// Right size (with new);
	catched = false;
	try {
		Buffer* b5 = new Buffer (1000);
		assert(b5->getSize() == 1000);
		assert(b5->getBuffer() != NULL);
		delete b5;
	} catch (...) {
		std::cerr << "6. ERROR: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == false);

	// Out of boundary
	catched = false;
	try {
		Buffer b6 (5);
		assert(b6.getBuffer() != NULL);
		const char* s = "buffer test";
		b6.fill (s, 11);
	} catch (...) {
		std::cerr << "7. OK: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == true);

	// Compare
	catched = false;
	try {
		Buffer b7 (50);
		const char* s1 = "ABCDEFGHILMNOPQRSTUVZ";
		const char* s2 = "ABCDEFGHILMNOPQRSTUVZ";
		b7.fill(s1, 21);
		Buffer b8 (100);
		b8.fill(s2, 21);
		assert(b8.compare(&b7, 21) == true);
		const char* s3 = "BCDEFGHIL";
		b8.fill (s3, 5);
		assert(b8.compare(&b7, 21) == false);
		b8.fill(&b7, 21);
		assert(b8.compare(&b7, 21) == true);
	} catch (...) {
		std::cerr << "8. ERROR: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == false);
}


void testFifoDescriptor()
{
	unlink("/tmp/test-fifo-1");
	FifoDescriptor fd("/tmp/test-fifo-1", O_WRONLY|O_CREAT, S_IRWXU);
	std::cout << "Fifo size: " << fd.getCapacity() << std::endl;
}


void testFileDescriptor()
{
	unlink("/tmp/test-file-1");

	bool catched = false;
	try {
		FileDescriptor fd1("/tmp/test-file-1", O_WRONLY);
	} catch (...) {
		std::cerr << "1. OK: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == true);

	catched = false;
	try {
		FileDescriptor fd2("/tmp/test-file-1", O_WRONLY|O_CREAT,
		    S_IRWXU);
		const char* s1 = "ABCDEFGHIL";
		Buffer b1(10);
		b1.fill(s1, 10);
		fd2.write(&b1, 10);
		fd2.close();
	} catch (...) {
		std::cerr << "2. ERROR: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == false);

	catched = false;
	try {
		FileDescriptor fd3("/tmp/test-file-1", O_WRONLY|O_APPEND);
		const char* s2 = "MNOPQRSTUVZ";
		fd3.write(s2, 11);
		fd3.close();
	} catch (...) {
		std::cerr << "3. ERROR: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == false);

	catched = false;
	try {
		FileDescriptor fd4("/tmp/test-file-1", O_RDONLY);
		Buffer b(21);
		fd4.read(&b, 21);
		const char* s3 ="ABCDEFGHILMNOPQRSTUVZ";
		assert(b.compare(s3, 21) == true);
		FileDescriptor fd5 = fd4;
		FileDescriptor fd6 ("/tmp/test", O_RDONLY|O_CREAT, S_IRWXU);
		fd6 = fd5;
		fd4.close();
	} catch (...) {
		std::cerr << "4. ERROR: exception catched" << std::endl;
		catched = true;
	}
	assert (catched == false);

}


void testLogger()
{
	// NO INITIALIZED

	// Not configured Logger
	DEBUG(DBG_WARN, "1. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_ERROR, "2. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "3. Error if seen");
	EXPECT(false, false);

	// NO PRINT

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_off|Logger::screen_off, DBG_DEBUG, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "4. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_ERROR, "5. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "6. Error if seen");
	EXPECT(false, false);

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_off, DBG_ERROR, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "7. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "8. Error if seen");
	EXPECT(false, false);

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_off, DBG_WARN, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "9. Error if seen");
	EXPECT(false, false);

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_off|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "10. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "11. Error if seen");
	EXPECT(false, false);

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_off|Logger::screen_on, DBG_DEBUG, DBG_WARN);
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "12. Error if seen");
	EXPECT(false, false);

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_on, DBG_ERROR, DBG_ERROR);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "13. Error if seen");
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "14. Error if seen");
	EXPECT(false, false);

	// Configured to print nothing
	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_on, DBG_WARN, DBG_WARN);
	EXPECT(false, false);
	DEBUG(DBG_DEBUG, "15. Error if seen");
	EXPECT(false, false);

	// PRINT ONLY ON FILE

	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_off, DBG_DEBUG, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "16. Should not appear on screen");
	EXPECT(true, false);
	DEBUG(DBG_ERROR, "17. Should not appear on screen");
	EXPECT(true, false);
	DEBUG(DBG_DEBUG, "18. Should not appear on screen");
	EXPECT(true, false);

	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_off, DBG_WARN, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "19. Should not appear on screen");
	EXPECT(true, false);
	DEBUG(DBG_ERROR, "20. Should not appear on screen");
	EXPECT(true, false);

	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_off, DBG_ERROR, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_ERROR, "21. Should not appear on screen");
	EXPECT(true, false);

	// PRINT ONLY ON SCREEN

	DEBUG_CONF("/tmp/test-logger", Logger::file_off|Logger::screen_on, DBG_DEBUG, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "22. Should not appear on file");
	EXPECT(false, true);
	DEBUG(DBG_ERROR, "23. Should not appear on file");
	EXPECT(false, true);
	DEBUG(DBG_DEBUG, "24. Should not appear on file");
	EXPECT(false, true);

	DEBUG_CONF("/tmp/test-logger", Logger::file_off|Logger::screen_on, DBG_DEBUG, DBG_WARN);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "25. Should not appear on file");
	EXPECT(false, true);
	DEBUG(DBG_ERROR, "26. Should not appear on file");
	EXPECT(false, true);

	DEBUG_CONF("/tmp/test-logger", Logger::file_off|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
	EXPECT(false, false);
	DEBUG(DBG_ERROR, "27. Should not appear on file");
	EXPECT(false, true);

	// PRINT ON SCREEN AND FILE

	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_on, DBG_DEBUG, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "28. Should appear on both");
	EXPECT(true, true);
	DEBUG(DBG_ERROR, "29. Should appear on both");
	EXPECT(true, true);
	DEBUG(DBG_DEBUG, "30. Should appear on both");
	EXPECT(true, true);

	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_on, DBG_WARN, DBG_WARN);
	EXPECT(false, false);
	DEBUG(DBG_WARN, "31. Should appear on both");
	EXPECT(true, true);
	DEBUG(DBG_ERROR, "32. Should appear on both");
	EXPECT(true, true);

	DEBUG_CONF("/tmp/test-logger", Logger::file_on|Logger::screen_on, DBG_ERROR, DBG_DEBUG);
	EXPECT(false, false);
	DEBUG(DBG_ERROR, "33. Should appear on both");
	EXPECT(true, true);
}

class MyThread1: public AbstractThread {
        // Put thread arguments here
	int sock;
	struct sockaddr_un serv_addr;
 public:
	MyThread1(): sock(-1){
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, "/tmp/test-socket");
	}
        void run() {
		sleep(2);
		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		const char* b1 = "ABCDEFGHILMNOP";
		DEBUG(DBG_DEBUG, "Writing data to socket...");
		if (write (sock, b1, 10) != 10) {
			DEBUG(DBG_ERROR, "ERROR writing data to socket...");
		}
		sleep(2);
		const char* b2 = "XYZ";
		if (write (sock, b2, 3) != 3) {
			DEBUG(DBG_ERROR, "ERROR writing data to socket...");
		}
        }
 };


class SocketReader: public AbstractDescriptorReader {
	SocketServerDescriptor* s_;
 public:
        SocketReader(DescriptorsMonitor& dm, SocketServerDescriptor* s):
		AbstractDescriptorReader(dm), s_(s){
                                monitorDescriptor(*s);
        }
        ~SocketReader(){}
        virtual void dataAvailable(PosixDescriptor& descriptor) {
                Buffer buff(3);
                int ret = s_->read(&buff, buff.getSize());
                std::cerr << ret << " bytes read" << std::endl;
                if (descriptor.getDescriptorNumber() != s_->getDescriptorNumber())
                        DEBUG(DBG_ERROR, "Called with wrong descriptor!");

		if (!strncmp(buff.getBuffer(), "XYZ", 3)){
			DEBUG(DBG_DEBUG, "OK!");
		} else {
			DEBUG(DBG_ERROR, "ERROR in Socket!");
			std::cout << "Value read: ";
			for (int i=0; i < 3; ++i) {
				std::cout << buff[i];
			}
			std::cout << std::endl;
		}
	}
 };


void testThreadsAndSockets()
{
	unlink("/tmp/test-socket");
	DEBUG(DBG_DEBUG, "Creating socket...");
	SocketTcpServer serv("/tmp/test-socket");
	DEBUG(DBG_DEBUG, "Creating thread...");
	MyThread1 t;
	DEBUG(DBG_DEBUG, "Running thread...");
	t.start();
	SocketServerDescriptor des (serv);
	Buffer b (10);
	DEBUG(DBG_DEBUG, "Reading data from socket...");
	des.read(&b, b.getSize());
	if (!strncmp(b.getBuffer(), "ABCDEFGHIL", 10)){
		DEBUG(DBG_DEBUG, "OK!");
	} else {
		DEBUG(DBG_ERROR, "ERROR in Socket!");
		std::cout << "Value read: ";
		for (int i=0; i < 10; ++i) {
			std::cout << b[i];
		}
		std::cout << std::endl;
	}
	DescriptorsMonitor dm;
	SocketReader sr(dm, &des);
	dm.wait();
}

void testTime()
{
	Time a;
	Time b;
	std::cout << "A: " << a.getSeconds() << "\t" << a.getUSeconds()
	    << std::endl;
	std::cout << "B: " << b.getSeconds() << "\t" << b.getUSeconds()
	    << std::endl;
	if ((a < b) || (a == b)) {
		DEBUG(DBG_DEBUG, "1. Time: OK!");
	} else {
		DEBUG(DBG_ERROR, "1. Error in Time!");
	}
	a.resetToCurrentTime();	
	if (a > b) {
		DEBUG(DBG_DEBUG, "2. Time: OK!");
	} else {
		DEBUG(DBG_ERROR, "2. Error in Time!");
	}
	if ((a == a) && (b == b)) {
		DEBUG(DBG_DEBUG, "3. Time: OK!");
	} else {
		DEBUG(DBG_ERROR, "3. Error in Time!");
	}
}

int main ()
{
	testLogger();
	testBuffer();
	testFileDescriptor();
	testFifoDescriptor();
	testThreadsAndSockets();
	testTime();
	return 0;
}
