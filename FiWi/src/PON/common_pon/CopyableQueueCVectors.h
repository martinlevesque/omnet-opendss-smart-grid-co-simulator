/*
 * CopyableQueueCVectors.h
 *
 *  Created on: May 25, 2010
 *      Author: urban
 */

#ifndef COPYABLEQUEUECVECTORS_H_
#define COPYABLEQUEUECVECTORS_H_

#include <string>
#include <omnetpp.h>

/**
 * This class hold pointers to cOutVectors. This way
 * it can be copied by the default copy constructor.
 *
 * The drawback is that WE CANNOT DELETE/FREE memory in
 * the destructor. The only "safe" way to free, is in
 * the module/class that uses a CopyableQueueCVectors
 * instance. Thus the container object MUST call deleteVectors()
 * at some point.
 */
class CopyableQueueCVectors{
private:
	unsigned long numBytesSentPrev;
	simtime_t lastUpdateTime;
	/// Time window in which to count /second variables
	double granularity;
public:
	// Statistics
	unsigned long numBytesSent;
	unsigned long numBytesDropped;
	unsigned long numFramesSent;

	/**
	 *  Runtime Statistics
	 *
	 *  These are not going to be logged to
	 *  cOutVectors since analysis can be done
	 *  finally. But we need them for DBAs.
	 *
	 *  NOTE:
	 *  The real way** should be to log drop rate (DoR)
	 *  and data rate (DaR), then:
	 *   DoR + DaR = REQUESTED_DATARATE.
	 *
	 *  What we do is logging IncommingRate (IR)
	 *  or else REQUESTED_DATARATE thus:
	 *   IR - DaR = DoR
	 *
	 *  ** By "real way" we mean what we HAVE SEEN on
	 *  real network hardware.
	 *
	 */
	uint64_t numIncomingBitsOld;
	uint64_t numIncomingBits;
	uint64_t incomingBPS;

	cOutVector *numBytesSentVector;
	cOutVector *numBytesDroppedVector;
	cOutVector *numFramesSentVector;

public:
	CopyableQueueCVectors();
	virtual ~CopyableQueueCVectors();
	void recordVectors();
	void setVectorNames(std::string name);
	void deleteVectors();

	double getGranularity();
	void setGranularity(double gran);

	unsigned long getIncomingBPS();
};

#endif /* COPYABLEQUEUECVECTORS_H_ */

//-------------------------------------------------------------

#ifndef Q_CONTAINER_H
#define Q_CONTAINER_H

/**
 * This class contains all the common variables that a
 * dynamically created queue will need. These include the
 * CopyableQueueCVectors (vec) and QoS parameters. The owner
 * module MUST call the clean() function to de-alloc memory
 * from the vectors.
 *
 * KNOWN BUG: Queue name does not always change... (for the gui)
 *
 */
class QForContainer : public cPacketQueue{
private:
	std::string srvName;
public:

	double prior;

	// QoS Control
	/// Maximum data rate (per second)
	uint64_t datarate_max;
	/// Bytes already sent (running second)
	uint64_t sentbits;
	// Queue Length
	int queueLimit;

	simtime_t lastUpdate;

	/// Stats
	CopyableQueueCVectors * vec;

	/// Basic Constructor
	QForContainer();
	QForContainer(const char * name);

	/// Clean everything
	void clean();
	/**
	 * This method is going to set the service name AND
	 * all the vector names AND the Queue name (for the qui).
	 *
	 * NOTE: It _must_ be called only once.
	 */
	void setServiceName(std::string name);

	std::string getServiceName();

	/**
	 * @Override
	 */
	virtual cPacket *pop();
	virtual cPacket *popNoLogging();

	/**
	 * Clears bits send
	 */
	virtual void clearBitsSend();

	/**
	 * Return the datarate till now
	 */
	virtual uint64_t getTxDataRate();
};

#endif
