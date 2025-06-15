#include "codel.h"
#include <algorithm>

CoDel::CoDel(SimTime target, SimTime interval)
    : target(target), interval(interval)
{
    if (target <= 0.0)
        throw cRuntimeError("CoDel: Invalid target value");
    if (interval <= 0.0)
        throw cRuntimeError("CoDel: Invalid interval value");

    reset();
}

void CoDel::reset() {
    dropping = false;
    dropNextTime = 0;
    firstAboveTime = 0;
    intervalStart = simTime();
    minDelay = SIMTIME_MAX;
    enqTimeQueue.clear();
}

void CoDel::packetEnqueued() {
    enqTimeQueue.push_back(simTime());
}

void CoDel::packetDequeued() {
    if (!enqTimeQueue.empty())
        enqTimeQueue.pop_front();
}

void CoDel::pushEnqueueTime(simtime_t t) {
    enqTimeQueue.push_back(t);
}

void CoDel::popEnqueueTime() {
    if (!enqTimeQueue.empty())
        enqTimeQueue.pop_front();
}

bool CoDel::shouldDrop(int queueLength, simtime_t enqTime) {
    if (enqTimeQueue.empty())
        return false;

    simtime_t now = simTime();
    simtime_t sojournTime = now - enqTimeQueue.front();

    // Update min delay during the interval
    if (sojournTime < minDelay)
        minDelay = sojournTime;

    // Evaluate after interval
    if (now - intervalStart >= interval) {
        if (minDelay > target) {
            if (!dropping) {
                dropping = true;
                dropNextTime = now + interval;
            }
        } else {
            dropping = false;
        }
        intervalStart = now;
        minDelay = sojournTime;
    }

    // If we're in dropping mode and it's time to drop
    if (dropping && now >= dropNextTime) {
        dropNextTime = now + interval;
        return true;
    }

    return false;
}
