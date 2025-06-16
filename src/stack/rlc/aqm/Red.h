/*
 * Red.h
 *
 *  Created on: Apr 14, 2025
 *      Author: almozaffar
 */

#ifndef STACK_RLC_AQM_RED_H_
#define STACK_RLC_AQM_RED_H_


#include <omnetpp.h>
#include <cmath>
#include <omnetpp/ccomponent.h>
#include <omnetpp/distrib.h>
#include "omnetpp/ccomponent.h"

using namespace omnetpp;
class Red : public cListener{
  private:
    double wq;             // Queue weight
    double minth;          // Minimum threshold
    double maxth;          // Maximum threshold
    double maxp;           // Maximum drop probability
    double pkrate;         // Estimated packet rate (packets/sec)
    int packetCapacity;    // Hard capacity
    double avg = 0.0;      // Average queue size
    int count = -1;        // Packet count since last drop
    simtime_t q_time;      // Time of last queue activity

  public:
    Red(double wq, double minth, double maxth, double maxp, double pkrate, int capacity);

    /**
     * Decide whether to drop a packet based on current queue length.
     * Returns true if the packet should be dropped.
     */
    bool shouldDrop(int queueLength);

    double getAvg() const { return avg; }
};


#endif /* STACK_RLC_AQM_RED_H_ */
