#ifndef LAB2_EVENT_H
#define LAB2_EVENT_H

# include "Process.h"

class Event {
public:
    Process *evtProcess;
    int evtTimeStamp;
    int evtTimeCreated;
    enum TRANS {
        TRANS_TO_READY,
        TRANS_TO_RUN,
        TRANS_TO_BLOCK,
        TRANS_TO_PREEMPT,
        UNKNOWN,
    } transition;

    Event(int timeStamp, int timeCreated, Process *proc, TRANS trans);

    struct DELETE_CMP : public std::unary_function<Event *, bool> {
        explicit DELETE_CMP(const Event &baseline) : baseline(baseline) {}

        bool operator()(Event *arg) const {
            return (arg->transition == TRANS_TO_BLOCK || arg->transition == TRANS_TO_READY) &&
                   arg->evtProcess == baseline.evtProcess;
        }

        const Event &baseline;
    };
};


#endif //LAB2_EVENT_H
