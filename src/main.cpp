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

/** @mainpage OnPosix Library
 * 
 * @author Evidence Srl - www.evidence.eu.com
 *
 * <h1>Introduction</h1>
 *
 * OnPosix is a tiny C++ library to offer POSIX features to C++
 * developers in a POSIX environment.
 *
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
 * To generate the documentation using Doxygen, type
 *
 * \code
 * make doc
 * \endcode
 *
 * <h1>Examples of usage</h1>
 *
 * Some examples about the characteristics offered by the library:
 *
 * <h2>Threads</h2>
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
 * \code
 * StreamSocketServer serv (1234);
 * StreamSocketServerDescriptor des (serv);
 * Buffer b (10);
 * des.read(b, b.getSize());
 * des.close();
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
 * <h2>Logging</h2>
 *
 * \code
 * DEBUG_CONF("outputfile", Logger::file_on|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
 * DEBUG(DBG_DEBUG, "hello " << "world");
 * \endcode
 *
 * <h2>Timing</h2>
 *
 * \code
 * Time t1;
 * std::cout << t1.getSeconds() << " " << t1.getUSeconds() << std::endl;
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
 */


