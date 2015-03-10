/**
 * @short Mobility module which leaves nodes at their starting positions
 * @author Isabel Dietrich
*/


#ifndef RANDOM_WP_MOBILITY_TRACE_H
#define RANDOM_WP_MOBILITY_TRACE_H

// SYSTEM INCLUDES
#include <omnetpp.h>
#include "RandomWPMobilityTrace.h"
#include "RandomWPMobility.h"

class INET_API RandomWPMobilityTrace : public RandomWPMobility
{
  public:
    // LIFECYCLE
    virtual void initialize(int);
    virtual void finish();
    virtual void setTargetPosition();

  private:
    // MEMBER VARIABLES
    cOutVector* mTrace;
    cOutVector* mTraceX;
    cOutVector* mTraceY;
};

#endif
