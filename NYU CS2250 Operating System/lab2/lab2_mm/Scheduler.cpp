#include "Scheduler.h"


Process *FCFS::getNextProcess() {
    Process *proc = nullptr;
    if (!readyQueue.empty()) {
        proc = readyQueue.front();
        readyQueue.pop();
    }
    return proc;
}

void FCFS::addProcess(Process *p) {
    readyQueue.push(p);
}

Process *LCFS::getNextProcess() {
    Process *proc = nullptr;
    if (!readyQueue.empty()) {
        proc = readyQueue.top();
        readyQueue.pop();
    }
    return proc;
}

void LCFS::addProcess(Process *p) {
    readyQueue.push(p);
}

Process *SRTF::getNextProcess() {
    Process *proc = nullptr;
    if (!readyQueue.empty()) {
        proc = readyQueue.top();
        readyQueue.pop();
    }
    return proc;
}

void SRTF::addProcess(Process *p) {
    readyQueue.push(p);
}

PRIO::PRIO(int maxprios) {
    maxPriority = maxprios;
    activeQ = new std::vector<std::queue<Process *>>(maxprios, std::queue<Process *>());
    expiredQ = new std::vector<std::queue<Process *>>(maxprios, std::queue<Process *>());
}

void PRIO::addProcess(Process *proc) {
    if (proc->dynamicPriority < 0) {
        proc->resetDynamicPriority();
        expiredQ->at(proc->dynamicPriority).push(proc); // enter into expireQ
    } else {
        activeQ->at(proc->dynamicPriority).push(proc);
    }
}

Process *PRIO::getNextProcess() {
    Process *proc;
    for (int i = maxPriority - 1; i >= 0; i--) {
        if (activeQ->at(i).empty()) {
            continue;
        } else {
            proc = activeQ->at(i).front();
            activeQ->at(i).pop();
            return proc;
        }
    }
    std::swap(activeQ, expiredQ);
    for (int i = maxPriority - 1; i >= 0; i--) {
        if (activeQ->at(i).empty()) {
            continue;
        } else {
            proc = activeQ->at(i).front();
            activeQ->at(i).pop();
            return proc;
        }
    }
    return nullptr;
}
