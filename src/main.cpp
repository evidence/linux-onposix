/*
 * main.cpp
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

/** 
 *
 * \htmlonly
 * <br>
 * <center>
 * <img src="onposix.png" width="250"></img><br>
 * Minimal Posix C++ library
 * </center>
 * <br>
 * \endhtmlonly
 *
 * @mainpage OnPosix Library
 * 
 * @author Evidence Srl - <a href="http://www.evidence.eu.com" target="_blank">www.evidence.eu.com</a>
 *
 * <br>
 * <br>
 * <h1>Introduction</h1>
 *
 * OnPosix is a tiny library to abstract POSIX mechanisms to C++
 * developers.<br>
 * <br>
 * Most features offered by this library can be found either inside the
 * <a href="http://www.boost.org" target="_blank">Boost library</a>
 * or in a standard library compliant with the
 * <a href="http://www.stroustrup.com/C++11FAQ.html">C++11 standard</a>.<br>
 * Unfortunately, however, for some embedded Linux devices, these libraries
 * cannot represent viable solutions, due to the lack of memory space
 * (for the Boost libraries) and the lack of a new C++ compiler
 * (e.g., on Xilinx MicroBlaze).
 * On these platforms, the OnPosix library represents a good and cheap solution
 * to have object-oriented POSIX mechanisms.<br>
 * <br>
 * The library is available both as a shared (.so) and a static
 * (.a) library. 
 * 
 *
 * <br>
 * <br>
 * <br>
 * <h1>License</h1>
 *
 * The library has been developed in the context of the
 * <a href="http://www.era-project.org">ERA</a> project, financially supported
 * by the European commission under the FP7 program.
 * <br>
 * <br>
 * The library is licensed under version 2 of the
 * <a href="http://www.gnu.org/licenses/old-licenses/lgpl-2.0.html">GNU LIBRARY
 * GENERAL PUBLIC LICENSE (LGPL)</a>.<br>
 * See file LICENSE for more details.
 *
 * <br>
 * <br>
 * <br>
 * <h1>Download</h1>
 *
 * Type
 * \code
 * git clone git://git.code.sf.net/p/onposix/code onposix-code
 * \endcode
 *
 * or <a href="http://sourceforge.net/projects/onposix/files/">
 * download a zip archive from Sourceforge</a>.
 *
 * <br>
 * <br>
 * <h1>Installation</h1>
 *
 * To build the library, just type:
 *
 * \code
 * make
 * \endcode
 *
 * The library is built both as shared library (.so) and static library (.a).
 *
 * To install the library on your system, type
 *
 * \code
 * make install
 * \endcode
 *
 * To install the library in a specific directory, type
 *
 * \code
 * make TARGET_DIR=/absolute/path install
 * \endcode
 *
 * Documentation is created through <a href="http://www.doxygen.org">Doxygen</a>.
 * To generate the documentation, type
 *
 * \code
 * make doc
 * \endcode
 *
 *
 * Unit testing is done through the <a href="http://code.google.com/p/googletest/">Google C++ Testing Framework</a>.
 * To test the library, specify the googletest path by setting the GTEST_INCLUDE_DIR and GTEST_LIB_DIR variables in the Makefile and type
 *
 * \code
 * make test
 * ./test
 * \endcode
 *
 * <br>
 * <br>
 * <h1>Examples of usage</h1>
 *
 * Some examples about the characteristics offered by the library:
 *
 * <h2>Threads</h2>
 *
 * The library offers basic mechanisms to create, start and stop a thread.<br>
 * It makes also possible to set scheduling parameters, thread affinity and signal handling.<br>
 * For threads, inherit a class from AbstractThread and specify a run() method.
 * Here we provide a small example. Information about all available methods can be found in the \ref onposix::AbstractThread class.<br>
 *
 * \code
 * class MyThread: public AbstractThread {
 *	// Put thread arguments here
 * public:
 *	MyThread() {
 *		// Initialize arguments here
 *      }
 *	void run() {
 *		// Put thread code here
 *		// Access arguments as normal attributes
 *	}
 * };
 *
 * int main ()
 * {
 *	MyThread t;
 *	t.blockSignal(SIGTERM);
 *	t.start();
 *	t.waitForTermination();
 * }
 * \endcode
 *
 * <h2>Mutual exclusion</h2>
 *
 * \code
 * PosixMutex m;
 * m.lock();
 * //....
 * m.unlock();
 * \endcode
 *
 * <h2>Condition variables</h2>
 *
 * \code
 * PosixCondition c;
 * PosixMutex m;
 * c.wait(&m);
 * //....
 * c.signal();
 * \endcode
 *
 * <h2>File descriptors</h2>
 *
 * \code
 * FileDescriptor fd ("/tmp/myfile", O_RDONLY);
 * Buffer b (10);
 * fd.read (b, b.getSize());
 * fd.close();
 * \endcode
 *
 * <h2>Socket descriptors</h2>
 *
 * The library offers mechanisms for both connection-oriented (e.g., TCP) and connection-less (e.g., UDP) communications.
 * Communications can be either AF_UNIX or AF_INET.
 * Here is a small example about a TCP client-server application.
 * More information can be found in the description of classes \ref onposix::StreamSocketServer, \ref onposix::StreamSocketServerDescriptor,
 * \ref onposix::StreamSocketClientDescriptor, \ref onposix::DgramSocketClientDescriptor and \ref onposix::DgramSocketServerDescriptor.<br>
 *
 * Server-side:
 * \code
 * int port = 1234;
 * StreamSocketServer serv (port);
 * while (true) {
 *	StreamSocketServerDescriptor des (serv);
 *	Buffer b (10);
 *	des.read(b, b.getSize());
 *	// ...
 * }
 * \endcode
 *
 * Client-side:
 * \code
 * int port = 1234;
 * std::string address = "192.168.10.133";
 * StreamSocketClientDescriptor des(address, port);
 * Buffer b (10);
 * des.write(b, b.getSize());
 * \endcode
 *
 * <h2>Pipes</h2>
 *
 * \code
 * Pipe p;
 * p.read(b, b.getSize());
 * p.close();
 * \endcode
 *
 *
 * <h2>FIFOs (AKA "named pipes")</h2>
 *
 * \code
 * FifoDescriptor fd ("/tmp/myfifo", O_RDONLY);
 * Buffer b (10);
 * fd.read (b, b.getSize());
 * \endcode
 *
 *
 *
 * <h2>Logging</h2>
 *
 * The whole library is built on top of a Logger that must be inizialized at the very beginning.
 * Log messages can be printed to console and/or to a file with different thresholds
 * of "verbosity".
 * 
 * \code
 * DEBUG_CONF("outputfile", Logger::file_on|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
 * DEBUG(DBG_DEBUG, "hello " << "world");
 * DEBUG(DBG_WARN, "something " << "strange");
 * DEBUG(DBG_ERR, "this is an error");
 * \endcode
 *
 * <h2>Timing</h2>
 *
 * \code
 * Time t1;
 * std::cout << t1.getSeconds() << " " << t1.getNSeconds() << std::endl;
 * \endcode
 *
 * <h2>Watching multiple descriptors</h2>
 *
 * \code
 * class FileReader: public AbstractDescriptorReader {
 *	FileDescriptor fd1_;
 *	FileDescriptor fd2_;
 * public:
 *	FileReader(){
 *		//...
 *		monitorDescriptor(fd1_);
 *		monitorDescriptor(fd2_);
 *	}
 *	void dataAvailable(PosixDescriptor& des) {
 *		std::cout << des.getDescriptorNumber() << "ready" << std::endl;
 *	}
 * }; 
 * \endcode
 *
 * <h2>Assertions</h2>
 *
 * Assertions provided by this library work also when code is compiled with the
 * -NDEBUG macro.
 *
 *  Example of usage:
 *  \code
 *  int i;
 *  VERIFY_ASSERTION(i == 0);
 *  \endcode
 *
 *
 * <h2>Much more...</h2>
 *
 * The library also offer Observer designer pattern on descriptors (i.e., \ref onposix::DescriptorsMonitor), buffers (i.e., \ref onposix::Buffer)
 * and shared queues (i.e., \ref onposix::PosixSharedQueue and \ref onposix::PosixPrioritySharedQueue).
 *
 * <br>
 * <br>
 * <h1>Support</h1>
 *
 * A <a href="http://sourceforge.net/p/onposix/mailman/">mailing list</a>
 * is available on Sourceforge. Fell free to propose feedback, new features and possible improvements.
 */


