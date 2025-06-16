/*
 * Red.cpp
 *
 *  Created on: Apr 14, 2025
 *      Author: almozaffar
 */

#include "Red.h"
#include <omnetpp/distrib.h>
#include "omnetpp/ccomponent.h"


Red::Red(double wq, double minth, double maxth, double maxp, double pkrate, int capacity)
    : wq(wq), minth(minth), maxth(maxth), maxp(maxp), pkrate(pkrate), packetCapacity(capacity), q_time(simTime())
{
    if (wq < 0.0 || wq > 1.0)
        throw cRuntimeError("RED: Invalid wq value");
    if (minth < 0 || maxth < 0 || minth >= maxth)
        throw cRuntimeError("RED: Invalid minth/maxth values");
    if (maxp < 0.0 || maxp > 1.0)
        throw cRuntimeError("RED: Invalid maxp value");
    if (pkrate <= 0.0)
        throw cRuntimeError("RED: Invalid pkrate value");
}

bool Red::shouldDrop(int queueLength)
{
    if (queueLength > 0) {
        avg = (1 - wq) * avg + wq * queueLength;
    } else {
        double m = SIMTIME_DBL(simTime() - q_time) * pkrate;
        avg = std::pow(1 - wq, m) * avg;
        q_time = simTime();
    }

    if (queueLength >= packetCapacity)
        return true;

    if (avg < minth) {
        count = -1;
        return false;
    } else if (avg >= maxth) {
        count = 0;
        return true;
    } else {
        count++;
        double pb = maxp * (avg - minth) / (maxth - minth);
        double pa = pb / (1 - count * pb);
        if (getEnvir()->getRNG(0)->doubleRand() < pa)
 {
            count = 0;
            return true;
        }
        return false;
    }
}


