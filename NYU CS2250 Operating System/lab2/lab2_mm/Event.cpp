#include "Event.h"

Event::Event(int timeStamp, int timeCreated, Process *proc, TRANS trans) : evtTimeStamp(timeStamp),
                                                                           evtTimeCreated(timeCreated),
                                                                           evtProcess(proc),
                                                                           transition(trans) {
}


