//
// VoipConfigs.h
//
// Auteurs : Martin Lévesque et François Lévesque
//
// Date : Novembre 2011

#ifndef VOIPCONFIGS_H_
#define VOIPCONFIGS_H_

#include <omnetpp.h>

class VoipConfigs : public cSimpleModule
{
protected:
	void initialize(int stage);
	virtual int numInitStages() const {return 4;}
	virtual void finish();

public:

	static double sumTCPDelays;
	static long nbTCPDelays;
	static double sumTCPThroughput;

	static std::string codec;
	static int nbVoipClients;
	static int nbDataClients;

	VoipConfigs();
	virtual ~VoipConfigs();
};

#endif /* VOIPCONFIGS_H_ */
