#ifndef LAB2_DES_H
#define LAB2_DES_H

# include <vector>
# include <utility>
# include <string>
# include <fstream>
# include <sstream>
# include <iterator>
# include <iostream>
# include <cassert>
# include <algorithm>
# include "Utils.h"
# include "Event.h"
# include "Scheduler.h"

class DES {
public:
    std::ifstream input_file;
    std::ifstream rand_file;
    int max_priority;
    int quantum;
    int verbose;

private:
    std::vector<Event *> eventQueue;
    std::vector<Process *> processes;

    Scheduler *scheduler;
    std::string schedName;


    int CURRENT_TIME = 0;
    bool CALL_SCHEDULER = false;
    bool PREEMPT = false;
    Process *CURRENT_RUNNING_PROCESS = nullptr;

    std::vector<std::pair<int, int>> CPUUtil;
    std::vector<std::pair<int, int>> IOUtil;

public:
    DES(const std::string &, const std::string &, int, char, int, int);

    void initialization();

    void simulation();

    ~DES();

    void report();

    int myRandom(int burst);

    int myRandom();

private:
    Event *getEvent();

    void putEvent(Event *evt);

    int getNextEventTime();

    int calCPUUtilTime();

    int calIOUtilTime();

    void removeEvent(const Event &evt);
};


#endif //LAB2_DES_H
