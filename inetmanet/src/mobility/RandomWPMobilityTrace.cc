#include "RandomWPMobilityTrace.h"

Define_Module(RandomWPMobilityTrace);

/////////////////////////////// PUBLIC ///////////////////////////////////////

//============================= LIFECYCLE ===================================
/**
 * Initialization routine
 */
void RandomWPMobilityTrace::initialize(int aStage)
{
    RandomWPMobility::initialize(aStage);

    if (aStage != 0) return;

    EV << "initializing RandomWPMobilityTrace stage " << aStage << endl;

    //mTrace = new cOutVector("Mobility Trace", 2);
    mTraceX = new cOutVector("Mobility Trace x");
    mTraceY = new cOutVector("Mobility Trace y");
}

void RandomWPMobilityTrace::finish()
{
    RandomWPMobility::finish();

    //delete mTrace;
    delete mTraceX;
    delete mTraceY;
}

void RandomWPMobilityTrace::setTargetPosition()
{
    RandomWPMobility::setTargetPosition();

    // record newly set target position and time
    //mTrace->record(targetPos.x, targetPos.y);

    mTraceX->record(targetPos.x);
    mTraceY->record(targetPos.y);
}
