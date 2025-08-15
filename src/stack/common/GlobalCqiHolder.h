// --- PASTE THIS INTO "src/stack/common/GlobalCqiHolder.h" ---

#ifndef STACK_COMMON_GLOBALCQIHOLDER_H_
#define STACK_COMMON_GLOBALCQIHOLDER_H_

#include <omnetpp.h>

// This is a simple class to hold our shared CQI value.
// It inherits from cObject so OMNeT++ can manage it.
class CqiHolder : public omnetpp::cObject
{
  public:
    double lastUeCqi_ = -1.0; // The shared CQI value, initialized to -1
};

#endif /* STACK_COMMON_GLOBALCQIHOLDER_H_ */