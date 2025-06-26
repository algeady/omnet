#ifndef STACK_RLC_AQM_CODEL_H_
#define STACK_RLC_AQM_CODEL_H_

#include <omnetpp.h>
#include <deque>
#include <cmath> // For std::sqrt

using namespace omnetpp;

class CoDel : public cListener {
  public:
    // Constructor called by UmTxEntity::initialize()
    CoDel(SimTime target, SimTime interval);

    // --- Public methods for UmTxEntity compatibility ---
    void reset();

    // Records the arrival time of a packet
    void pushEnqueueTime(simtime_t t);

    // Removes the timestamp of a dropped packet
    void popEnqueueTime();

    // The main decision function, adapted to be called at enqueue time
    bool shouldDrop(int queueLength, simtime_t arrivalTime);

    // --- NEW METHOD ---
        // Notifies CoDel that a packet was successfully removed from the main queue.
        void packetDequeued();

  private:
    // --- Internal state based on RFC 8289 ---
    SimTime target_;
    SimTime interval_;
    simtime_t first_above_time_;
    simtime_t drop_next_;
    uint32_t count_;
    uint32_t lastcount_;
    bool dropping_;

    // --- Timestamp tracking ---
    // This queue stores the arrival times of packets currently in the buffer.
    std::deque<simtime_t> enq_time_queue_;

    // --- Helper method from RFC 8289 ---
    simtime_t control_law(simtime_t t, uint32_t c);
};

#endif // STACK_RLC_AQM_CODEL_H_
