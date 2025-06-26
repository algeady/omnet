#include "codel.h"

CoDel::CoDel(SimTime target, SimTime interval)
    : target_(target), interval_(interval) {
    if (target_ <= SIMTIME_ZERO)
        throw cRuntimeError("CoDel: Invalid target value. Must be positive.");
    if (interval_ <= SIMTIME_ZERO)
        throw cRuntimeError("CoDel: Invalid interval value. Must be positive.");
    reset();
}

void CoDel::reset() {
    first_above_time_ = 0;
    drop_next_ = 0;
    count_ = 0;
    lastcount_ = 0;
    dropping_ = false;
    enq_time_queue_.clear();
}

void CoDel::pushEnqueueTime(simtime_t t) {
    // This is now essential. It records the arrival time.
    enq_time_queue_.push_back(t);
}

void CoDel::popEnqueueTime() {
    // This is also essential for rolling back a timestamp if a drop occurs.
    if (!enq_time_queue_.empty()) {
        enq_time_queue_.pop_back();
    }
}

bool CoDel::shouldDrop(int queueLength, simtime_t arrivalTime) {
    // NOTE: The arguments 'queueLength' and 'arrivalTime' from UmTxEntity are ignored.
    // The decision logic MUST use the state of the packet at the HEAD of the queue.

    if (enq_time_queue_.empty()) {
        return false; // Cannot make a decision if the queue is empty
    }

    simtime_t now = simTime();

    // Calculate sojourn time for the packet at the HEAD of the queue.
    simtime_t sojourn_time = now - enq_time_queue_.front();

    bool ok_to_drop = false;

    // This section corresponds to the logic in 'dodequeue' from the RFC.
    // It determines if the queue has been persistently above the target.
    if (sojourn_time < target_ || enq_time_queue_.size() <= 1) {
        first_above_time_ = 0;
    } else {
        if (first_above_time_ == 0) {
            first_above_time_ = now + interval_;
        } else if (now >= first_above_time_) {
            ok_to_drop = true;
        }
    }

    // This section corresponds to the main 'dequeue' logic from the RFC.
    if (dropping_) {
        if (!ok_to_drop) {
            dropping_ = false;
        } else if (now >= drop_next_) {
            count_++;
            drop_next_ = control_law(drop_next_, count_);
            return true; // Decision: DROP
        }
    } else if (ok_to_drop) {
        dropping_ = true;

        uint32_t delta = count_ - lastcount_;
        count_ = 1;
        if ((delta > 1) && (now - drop_next_ < 16 * interval_)) {
            count_ = delta;
        }

        drop_next_ = control_law(now, count_);
        lastcount_ = count_;
        return true; // Decision: DROP
    }

    return false; // Decision: KEEP
}


// --- NEW IMPLEMENTATION ---
void CoDel::packetDequeued() {
    // A packet was successfully sent, so remove its corresponding timestamp
    // from the front of our internal tracking queue.
    if (!enq_time_queue_.empty()) {
        enq_time_queue_.pop_front();
    }
}



simtime_t CoDel::control_law(simtime_t t, uint32_t c) {
    if (c > 0) {
        return t + (interval_.dbl() / std::sqrt(c));
    }
    // Fallback for count=0, though it shouldn't be called with it.
    return t + interval_;
}
