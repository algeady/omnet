// CoDel.h
#ifndef STACK_RLC_AQM_CODEL_H_
#define STACK_RLC_AQM_CODEL_H_

#include <omnetpp.h>
#include <deque>

using namespace omnetpp;

class CoDel : public cListener {
  private:
    SimTime target;           // Desired max queuing delay
    SimTime interval;         // Time window to evaluate delay
    simtime_t firstAboveTime = 0;
    simtime_t intervalStart = 0;
    simtime_t minDelay = SIMTIME_MAX;

    bool dropping = false;
    simtime_t dropNextTime = 0;

    std::deque<simtime_t> enqTimeQueue; // Timestamps of packets

  public:
    CoDel(SimTime target, SimTime interval);

    void reset();

    // Track enqueuing and dequeuing
    void packetEnqueued();
    void packetDequeued();

    void pushEnqueueTime(simtime_t t);
    void popEnqueueTime();

    // CoDel decision
    bool shouldDrop(int queueLength, simtime_t enqTime);
};

#endif /* STACK_RLC_AQM_CODEL_H_ */
