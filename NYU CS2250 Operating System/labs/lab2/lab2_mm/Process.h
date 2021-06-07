#ifndef LAB2_PROCESS_H
#define LAB2_PROCESS_H

# include <vector>
# include <utility>
# include "Utils.h"

class Process {
public:
    Process(int pid, int at, int tc, int cb, int io, int sp);

    int PID;
    int arrivalTime; // AT
    int totalCPUTime; // TC
    int CPUBurst; // CB
    int IOBurst; // IO
    int staticPriority;
    int dynamicPriority;

    enum STATES {
        CREATED, READY, RUNNING, BLOCKED
    } currentState = CREATED;
    int stateTimeStamp;
    int currentCB = 0;

    void resetDynamicPriority() { dynamicPriority = staticPriority - 1; }

    int getCurrentCb() const;

    int getStateTimeStamp() const;

    void setStateAndStateTimeStamp(STATES s, int ts);

    STATES getCurrentState() const;

    std::vector<std::pair<STATES, int>> lifecycle;

    bool finish() const { return cpuTime() == totalCPUTime; };

    static int myRandom(int seed, int burst) { return 1 + (seed % burst); }

    int cpuTime() const;

    int remCPUTime() const { return totalCPUTime - cpuTime(); }

    int finishTime();

    int IOWaitTime();

    int CPUWaitTime();

    int startNewRun(int seed);

    int block(int seed);

    void finishRun(int time);

    void finishBlock(int time);

    void finishReady(int time);
};


#endif //LAB2_PROCESS_H
