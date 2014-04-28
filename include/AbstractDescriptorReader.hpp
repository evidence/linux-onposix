/*
 * AbstractDescriptorReader.hpp
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

#ifndef ABSTRACTDESCRIPTORREADER_HPP_
#define ABSTRACTDESCRIPTORREADER_HPP_

#include "DescriptorsMonitor.hpp"
#include "PosixDescriptor.hpp"

namespace onposix {

/**
 * \brief Abstract class to be notified when a descriptor becomes ready for
 * read operations.
 *
 * "Observer" class to monitor readiness of a PosixDescriptor.
 * This class allows to create a subclass which is notified when a descriptor
 * becomes ready for read operations.
 * Inherit from this class if your class needs to monitor one or more
 * (file or socket) descriptors for readiness to read operations.
 * The constructor receives as argument a DescriptorsMonitor class, which is
 * the class in charge of monitoring the status of all the given descriptors,
 * and notifying (through a call to the dataAvailable() method) the readiness
 * of a specific descriptor.
 * This class, together with DescriptorsMonitor, implements the "Observer"
 * design pattern.
 *
 * Example of class notified when data are available reading from a file:
 * \code
 * class FileReader: public AbstractDescriptorReader {
 * 	FileDescriptor fd_;
 * 	std::string name_;
 * 	int calledTimes_;
 * public:
 * 	FileReader(DescriptorsMonitor& dm,
 * 		const std::string& name,
 * 		const std::string& filename):
 * 			AbstractDescriptorReader(dm),
 * 			fd_(filename, O_RDONLY),
 * 			name_(name),
 * 			calledTimes_(0){
 * 				monitorDescriptor(fd_);
 *	}
 * 	~FileReader(){}
 * 	void dataAvailable(PosixDescriptor& descriptor) {
 * 		calledTimes_++;
 * 		Buffer buff(100);
 * 		std::cerr << "Name: " << name_
 * 			<< "\tDescriptor: " << descriptor
 * 			<< "\tTimes: " << calledTimes_ << std::endl;
 * 		int ret = fd_.read(&buff, buff.getSize());
 * 		std::cerr << ret << "bytes read" << std::endl;
 * 		if (descriptor.getDescriptorNumber() !=
 *		    fd_.getDescriptorNumber())
 * 			ERROR("Called with wrong descriptor!");
 * 	}
 * };
 * \endcode
 */
class AbstractDescriptorReader {
	/**
	 * \brief Monitor that will notify the readiness of descriptors.
	 *
	 * This is the monitor which, once monitorDescriptor(int descriptor)
	 * has been called, will automatically call 
	 * dataAvailable(int descriptor)
	 * whenever such a descriptor becomes available for read operations.
	 */
	DescriptorsMonitor* dm_;

public:
	/**
	 * \brief Constructor.
	 *
	 * It just saves the pointer of the DescriptorsMonitor, that will be
	 * used to register at the monitor when monitorDescriptor() will be
	 * called.
	 * @param reference to the DescriptorsMonitor
	 */
	explicit AbstractDescriptorReader(DescriptorsMonitor& dm):dm_(&dm){}

	virtual ~AbstractDescriptorReader(){}


	/**
	 * \brief Method called when the descriptor becomes ready
	 *
	 * Once monitorDescriptor(int descriptor) has been called, this
	 * method
	 * will be automatically called by the DescriptorsMonitor
	 * whenever the descriptor becomes available.
	 * @param Reference to the descriptor that became ready for read
	 * operations (the inherited class may want to be called when more than
	 * one descriptor becomes ready).
	 */
	virtual void dataAvailable(PosixDescriptor& descriptor)=0;

	/**
	 * \brief Method to start monitoring a descriptor.
	 *
	 * This method is usually called inside the constructor of the
	 * inherited
	 * class and allows to start monitoring a specific descriptor.
	 * @param Descriptor that must be monitored
	 * @return true in case of success, false otherwise
	 */
	inline bool monitorDescriptor(PosixDescriptor& descriptor){
		return dm_->startMonitoringDescriptor(*this, descriptor);
	}

	/**
	 * \brief Method to stop monitoring a descriptor.
	 *
	 * @param Descriptor that must be monitored
	 * @return true in case of success, false otherwise
	 */
	inline bool stopMonitorDescriptor(PosixDescriptor& descriptor){
		return dm_->stopMonitoringDescriptor(descriptor);
	}
};


} /* onposix */

#endif /* ABSTRACTDESCRIPTORREADER_HPP_ */
