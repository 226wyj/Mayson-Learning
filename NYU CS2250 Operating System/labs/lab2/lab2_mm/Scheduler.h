#ifndef LAB2_SCHEDULER_H
#define LAB2_SCHEDULER_H


#include <queue>
#include <stack>
#include "Process.h"


class Scheduler {
public:
    virtual void addProcess(Process *p) = 0;

    virtual Process *getNextProcess() = 0;

};

class FCFS : public Scheduler {
public:
    std::queue<Process *> readyQueue;

    Process *getNextProcess() final;

    void addProcess(Process *) final;

};

class LCFS : public Scheduler {
public:
    std::stack<Process *> readyQueue;

    Process *getNextProcess() final;

    void addProcess(Process *) final;

};

class SRTF : public Scheduler {
public:
    struct process_greater {
        bool operator()(Process *const a, Process *const b) {
            if (a->remCPUTime() == b->remCPUTime()) {
                return a->PID > b->PID;
            } else {
                return a->remCPUTime() > b->remCPUTime();
            }
        }
    };

    std::priority_queue<Process *, std::vector<Process *>, process_greater> readyQueue;

    Process *getNextProcess() final;

    void addProcess(Process *) final;
};

class PRIO : public Scheduler {
private:
    int maxPriority;
    std::vector<std::queue<Process *>> *activeQ;
    std::vector<std::queue<Process *>> *expiredQ;

public:
    explicit PRIO(int maxprios);

private:
    Process *getNextProcess() final;

    void addProcess(Process *) final;
};

#endif //LAB2_SCHEDULER_H
