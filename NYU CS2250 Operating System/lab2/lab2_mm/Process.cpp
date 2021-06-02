#include "Process.h"


Process::Process(int pid, int at, int tc, int cb, int io, int sp) : PID(pid), arrivalTime(at),
                                                                    totalCPUTime(tc),
                                                                    CPUBurst(cb), IOBurst(io),
                                                                    staticPriority(sp) {
    resetDynamicPriority();
    stateTimeStamp = at;
}

int Process::startNewRun(int seed) {
    if (currentCB == 0)
        currentCB = std::min(myRandom(seed, this->CPUBurst), totalCPUTime - cpuTime());
    return currentCB;
}

void Process::finishRun(int time) {
    currentCB -= time;
    lifecycle.emplace_back(RUNNING, time);
}

int Process::block(int seed) {
    int io_time = myRandom(seed, this->IOBurst);
//    currentState = BLOCKED;
    return io_time;
}

void Process::finishBlock(int time) {
    lifecycle.emplace_back(BLOCKED, time);
}

void Process::finishReady(int time) {
    lifecycle.emplace_back(READY, time);
}


int Process::finishTime() {
    int time = arrivalTime;
    if (this->finish()) {
        for (auto i: lifecycle) {
            time += i.second;
        }
        return time;
    } else {
        return -1;
    }
}

int Process::cpuTime() const {
    int time = 0;
    for (auto i: lifecycle) {
        if (i.first == RUNNING) time += i.second;
    }
    return time;
}

int Process::IOWaitTime() {
    int time = 0;
    for (auto i: lifecycle) {
        if (i.first == BLOCKED) time += i.second;
    }
    return time;
}

int Process::CPUWaitTime() {
    int time = 0;
    for (auto i: lifecycle) {
        if (i.first == READY) time += i.second;
    }
    return time;
}

Process::STATES Process::getCurrentState() const {
    return currentState;
}

int Process::getStateTimeStamp() const {
    return stateTimeStamp;
}

void Process::setStateAndStateTimeStamp(Process::STATES s, int ts) {
    currentState = s;
    Process::stateTimeStamp = ts;
}

int Process::getCurrentCb() const {
    return currentCB;
}


