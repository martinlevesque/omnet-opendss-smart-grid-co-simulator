/*
 * CopyableQueueCVectors.cpp
 *
 *  Created on: May 25, 2010
 *      Author: urban
 */

#include "CopyableQueueCVectors.h"

CopyableQueueCVectors::CopyableQueueCVectors() {
	numBytesSentVector = new cOutVector();
	numBytesDroppedVector = new cOutVector();
	numFramesSentVector = new cOutVector();
	numBytesSent = 0;
	numBytesDropped = 0;
	numFramesSent = 0;



	lastUpdateTime = simTime();
	granularity=1;
	numIncomingBits = numIncomingBitsOld = incomingBPS = 0;

}

CopyableQueueCVectors::~CopyableQueueCVectors() {
	// NOTE: DO NOT DELETE cOutVectors ...
	// This is called when you add a queue to
	// a vector.
}


// Vector Handling
void CopyableQueueCVectors::recordVectors(){
	numBytesSentVector->record(numBytesSent);
	numBytesDroppedVector->record(numBytesDropped);
	numFramesSentVector->record(numFramesSent);

	/*
	 In order to dynamically change the in queue size...
	 (we can log incoming Frame Rate)
	 Although I m not a Q expert...
	 http://www.opalsoft.net/qos/DS.htm explains much more
	 sophisticated queue types.
	 */
	/*
	 Hold stats per second
	 NOTE:	5x 10x and so on granularity can be implemented
			if needed. (OLT and general the DBA algorithm can
			internally do this)
	 */

	/**
	 * WARNING 0/0 = UNEXPECTED FUCKIN RESULT (OVERFLOWS STH)
	 */
	if (numIncomingBits - numIncomingBitsOld != 0 && simTime().raw() != lastUpdateTime.raw())
		incomingBPS = (numIncomingBits - numIncomingBitsOld)/(simTime() - lastUpdateTime);

	// INCOMMING
	if (simTime() - lastUpdateTime >= granularity){
		lastUpdateTime = simTime();
		numIncomingBitsOld = numIncomingBits;
		incomingBPS = 0;
	}
}

unsigned long CopyableQueueCVectors::getIncomingBPS(){
	return 0;
}

void CopyableQueueCVectors::setVectorNames(std::string name){
	std::string tmp_name;
	tmp_name = name + " bytesSent";
	numBytesSentVector->setName(tmp_name.c_str());
	tmp_name = name + " bytesDropped";
	numBytesDroppedVector->setName(tmp_name.c_str());
	tmp_name = name + " framesSent";
	numFramesSentVector->setName(tmp_name.c_str());
}
void CopyableQueueCVectors::deleteVectors(){
	delete numBytesSentVector;
	delete numBytesDroppedVector;
	delete numFramesSentVector;
}

double CopyableQueueCVectors::getGranularity(){
	return granularity;
}
void CopyableQueueCVectors::setGranularity(double gran){
	granularity = gran;
}

//---------------------------------------------------------------------------


QForContainer::QForContainer(){
	vec = new CopyableQueueCVectors();
	datarate_max=0;
	sentbits=0;
	prior=0;
	srvName="";
	queueLimit=1000;
}

QForContainer::QForContainer(const char * name){
	vec = new CopyableQueueCVectors();
	datarate_max=0;
	sentbits=0;
	prior=0;
	srvName="";
	queueLimit=1000;
	setServiceName(name);
}

void QForContainer::clean(){
	vec->deleteVectors();
	delete vec;
}

void QForContainer::setServiceName(std::string name){
	srvName=name;
	vec->setVectorNames(srvName);
	setName(srvName.c_str());
}

std::string QForContainer:: getServiceName(){
	return srvName;
}

cPacket * QForContainer::pop(){
	// Log Statistics
	vec->numBytesSent+=((cPacket *)front())->getByteLength();
	vec->numFramesSent++;
	vec->recordVectors();


	// Update Queue sent bytes
	sentbits+=((cPacket *)front())->getBitLength();
	return cPacketQueue::pop();
}

cPacket * QForContainer::popNoLogging(){
	return cPacketQueue::pop();
}

void QForContainer::clearBitsSend(){
	sentbits = 0;
	lastUpdate = simTime();
}

uint64_t QForContainer::getTxDataRate(){
	return sentbits/(simTime() - lastUpdate);
}


