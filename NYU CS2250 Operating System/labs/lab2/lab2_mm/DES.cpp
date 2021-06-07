#include "DES.h"

#include <utility>

void DES::simulation() {
    Event *evt;
    while (!eventQueue.empty()) {
        evt = getEvent();
        Process *proc = evt->evtProcess;
        CURRENT_TIME = evt->evtTimeStamp;
        int timeInPrevState = CURRENT_TIME - proc->stateTimeStamp;
        if (verbose)
            printf("%d %d %d: ", CURRENT_TIME, proc->PID, timeInPrevState);
        switch (evt->transition) {
            case Event::TRANS_TO_READY: {
                // must add to run queue
                switch (proc->currentState) {
                    case Process::CREATED:
                        proc->setStateAndStateTimeStamp(Process::READY, CURRENT_TIME);
                        if (verbose) printf("CREATED -> READY\n");
                        break;
                    case Process::BLOCKED: // come from BLOCKED
                        proc->finishBlock(timeInPrevState);
                        proc->resetDynamicPriority();
                        IOUtil.emplace_back(std::make_pair(evt->evtTimeCreated, evt->evtTimeStamp));
                        proc->setStateAndStateTimeStamp(Process::READY, CURRENT_TIME);
                        if (verbose) printf("BLOCK -> READY\n");
                        break;
                    case Process::RUNNING: // come from PREEMPTION
                        proc->finishRun(timeInPrevState);
                        CURRENT_RUNNING_PROCESS = nullptr;
                        CPUUtil.emplace_back(std::make_pair(evt->evtTimeCreated, evt->evtTimeStamp));
                        proc->setStateAndStateTimeStamp(Process::READY, CURRENT_TIME);
                        if (verbose) {
                            int rem = proc->totalCPUTime - proc->cpuTime();
                            printf("RUNNG -> READY  cb=%d rem=%d prio=%d\n", proc->currentCB, rem,
                                   proc->dynamicPriority);
                        }
                        if (schedName == "PRIO" || schedName == "PREPRIO")
                            proc->dynamicPriority--;
                        break;
                    default:
                        exit(-1);
                }

                if (schedName == "PREPRIO" && CURRENT_RUNNING_PROCESS != nullptr) {
                    bool preempt = CURRENT_RUNNING_PROCESS->dynamicPriority < proc->dynamicPriority;
                    int expectedTS = PREEMPT ? CURRENT_TIME : CURRENT_RUNNING_PROCESS->getStateTimeStamp() +
                                                              std::min(quantum,
                                                                       CURRENT_RUNNING_PROCESS->getCurrentCb());
                    if (CURRENT_TIME >= expectedTS)
                        preempt = false;

                    if (verbose) {
                        printf("---> PRIO preemption %d by %d ? %d TS=%d now=%d) --> %s\n",
                               CURRENT_RUNNING_PROCESS->PID,
                               evt->evtProcess->PID,
                               CURRENT_RUNNING_PROCESS->dynamicPriority < proc->dynamicPriority ? 1 : 0,
                               expectedTS,
                               CURRENT_TIME,
                               preempt ? "YES" : "NO");
                    }


                    if (preempt) {
                        PREEMPT = true;
                        putEvent(new Event(CURRENT_TIME, CURRENT_TIME, CURRENT_RUNNING_PROCESS,
                                           Event::TRANS_TO_PREEMPT));
                    }
                }

                scheduler->addProcess(proc);
                CALL_SCHEDULER = true;
                break;
            }

            case Event::TRANS_TO_RUN: {
                // create event for either preemption or blocking
                proc->finishReady(timeInPrevState);

//                int cpu_time = proc->startRun(myRandom());
                int cpu_time = proc->currentCB > 0 ? proc->currentCB : proc->startNewRun(myRandom());
                proc->setStateAndStateTimeStamp(Process::RUNNING, CURRENT_TIME);

                if (quantum >= cpu_time) {
                    putEvent(new Event(CURRENT_TIME + cpu_time, CURRENT_TIME, proc,
                                       Event::TRANS_TO_BLOCK)); // create event for blocking
                } else {
                    putEvent(new Event(CURRENT_TIME + quantum, CURRENT_TIME, proc,
                                       Event::TRANS_TO_READY)); // create event for preemption
                }
                if (verbose) {
                    int rem = proc->totalCPUTime - proc->cpuTime();
                    printf("READY -> RUNNG cb=%d rem=%d prio=%d\n", cpu_time, rem, proc->dynamicPriority);
                }
                break;
            }

            case Event::TRANS_TO_BLOCK: {
                // create an event when process becomes READY again
                // RUNNG -> BLOCK
                assert(proc->getCurrentState() == Process::RUNNING);
                proc->finishRun(timeInPrevState);
                CURRENT_RUNNING_PROCESS = nullptr;
                CPUUtil.emplace_back(std::make_pair(evt->evtTimeCreated, evt->evtTimeStamp));

                if (!proc->finish()) {
                    int io_time = proc->block(myRandom());
                    proc->setStateAndStateTimeStamp(Process::BLOCKED, CURRENT_TIME);
                    putEvent(new Event(CURRENT_TIME + io_time, CURRENT_TIME, proc, Event::TRANS_TO_READY));
                    if (verbose)
                        printf("RUNNG -> BLOCK  ib=%d rem=%d\n", io_time, proc->totalCPUTime - proc->cpuTime());

                } else {
                    if (verbose)
                        printf("Done\n");
                }
                CALL_SCHEDULER = true;
                break;
            }

            case Event::TRANS_TO_PREEMPT: {
                PREEMPT = false;
                removeEvent(Event(-1, proc->getStateTimeStamp(), proc, Event::UNKNOWN));
                proc->finishRun(timeInPrevState);
                CURRENT_RUNNING_PROCESS = nullptr;
                CPUUtil.emplace_back(std::make_pair(proc->stateTimeStamp, CURRENT_TIME));
                proc->setStateAndStateTimeStamp(Process::READY, CURRENT_TIME);

                if (verbose) {
                    int rem = proc->totalCPUTime - proc->cpuTime();
                    printf("RUNNG -> READY  cb=%d rem=%d prio=%d\n", proc->currentCB, rem,
                           proc->dynamicPriority);
                }
                if (schedName == "PRIO" || schedName == "PREPRIO")
                    proc->dynamicPriority--;

                scheduler->addProcess(proc);
                CALL_SCHEDULER = true;
                break;
            }
            default:
                break;

        }
        // remove current event object from memory
        delete evt;
        evt = nullptr;

        if (CALL_SCHEDULER) {
            if (getNextEventTime() == CURRENT_TIME) continue; // process next event from Event queue
            CALL_SCHEDULER = false; // reset global flag
            if (CURRENT_RUNNING_PROCESS == nullptr) {
                CURRENT_RUNNING_PROCESS = scheduler->getNextProcess();
                if (CURRENT_RUNNING_PROCESS == nullptr) {
                    continue;
                }
                // create event to make this process runnable for same time.
                putEvent(new Event(CURRENT_TIME, CURRENT_TIME, CURRENT_RUNNING_PROCESS, Event::TRANS_TO_RUN));
            }
        }
    }
}

Event *DES::getEvent() {
    Event *evt = nullptr;
    if (!eventQueue.empty()) {
        evt = eventQueue.front();
        eventQueue.erase(eventQueue.begin());
    }
    return evt;
}

void DES::putEvent(Event *evt) {
    auto l = eventQueue.begin();
    auto r = eventQueue.end();
    while (l != r) {
        // binary search
        auto m = l + (r - l) / 2;
        if ((*m)->evtTimeStamp > evt->evtTimeStamp) {
            r = m;
        } else {
            l = m + 1;
        }
    }
    eventQueue.insert(l, evt);
}

void DES::removeEvent(const Event &baseline) {
    auto position = std::find_if(eventQueue.begin(), eventQueue.end(), Event::DELETE_CMP(baseline));
    if (position != eventQueue.end())
        eventQueue.erase(position);
}

int DES::getNextEventTime() {
    if (!eventQueue.empty()) {
        return eventQueue.front()->evtTimeStamp;
    } else {
        return -1;
    }
}

DES::~DES() {
    //delete scheduler;
}

DES::DES(const std::string &infile, const std::string &rfile, int maxprio, char sched, int q, int v) :
        max_priority(maxprio), quantum(q), verbose(v) {
    rand_file.open(rfile);
    int numLine = myRandom();
    // assert(numLine == 40000);
    input_file.open(infile);

    switch (sched) {
        case 'F':
            scheduler = new FCFS();
            schedName = "FCFS";
            break;
        case 'L':
            scheduler = new LCFS();
            schedName = "LCFS";
            break;
        case 'S':
            scheduler = new SRTF();
            schedName = "SRTF";
            break;
        case 'R':
            scheduler = new FCFS();
            schedName = "RR";
            break;
        case 'P':
            scheduler = new PRIO(maxprio);
            schedName = "PRIO";
            break;
        case 'E':
            scheduler = new PRIO(maxprio);
            schedName = "PREPRIO";
            break;
        default:
            break;
    }


}

void DES::initialization() {
    std::string line;
    int counter = 0;
    while (input_file) {
        getline(input_file, line);
        if (line.empty()) continue;

        auto v = Utils::split(line, ' ');
        auto *proc = new Process(counter++,
                                 std::stoi(v[0]),
                                 std::stoi(v[1]),
                                 std::stoi(v[2]),
                                 std::stoi(v[3]),
                                 myRandom(max_priority));
        processes.emplace_back(proc);
    }
    for (auto *proc:processes) {
        auto *evt = new Event(proc->arrivalTime, proc->arrivalTime, proc, Event::TRANS_TO_READY);
        putEvent(evt);
    }

    CURRENT_TIME = processes[0]->arrivalTime;
}

void DES::report() {
    int pid; // unique id for this process
    int arrival; // AT: specified arrival time of process
    int totaltime; // TC: specified total exec time of process
    int cpuburst; // CB: specified cpuburst of process
    int ioburst; // IO: specified ioburst of process
    int static_prio; // PRIO: static priority of the process

    int finishtime; // FT: Finishing time
    int turnaroundtime; // TT: Turnaround time (FT-AT)
    int iowaittime; // IT: I/O Time (time in blocked state)
    int cpuwaittime; // CW: CPU Waiting time (time in ready state)

    int numprocess;
    int maxfintime; // Finishing time of the last event (i.e. the last process finished execution)
    double cpu_util; // CPU utilization (i.e. percentage (0.0 – 100.0) of time at least one process is running
    double io_util; // IO utilization (i.e. percentage (0.0 – 100.0) of time at least one process is performing IO
    double avg_turnaround = 0.0; // Average turnaround time among processes
    double avg_waittime = 0.0; // Average cpu waiting time among processes
    double throughput; // Throughput of number processes per 100 time units

    numprocess = processes.size();

    if (quantum == 10000)
        printf("%s\n", schedName.c_str());
    else
        printf("%s %d\n", schedName.c_str(), quantum);


    for (auto proc: processes) {
        pid = proc->PID;
        arrival = proc->arrivalTime;
        totaltime = proc->totalCPUTime;
        cpuburst = proc->CPUBurst;
        ioburst = proc->IOBurst;
        static_prio = proc->staticPriority;

        finishtime = proc->finishTime();
        turnaroundtime = finishtime - arrival;
        avg_turnaround += turnaroundtime;
        iowaittime = proc->IOWaitTime();
        cpuwaittime = proc->CPUWaitTime();
        avg_waittime += cpuwaittime;

        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
               pid, arrival, totaltime, cpuburst, ioburst, static_prio,
               finishtime, turnaroundtime, iowaittime, cpuwaittime);
    }


    maxfintime = CURRENT_TIME;
    avg_turnaround /= numprocess;
    avg_waittime /= numprocess;
    cpu_util = double(calCPUUtilTime()) / maxfintime * 100;
    io_util = double(calIOUtilTime()) / maxfintime * 100;
    throughput = double(numprocess) / maxfintime * 100;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           maxfintime,
           cpu_util,
           io_util,
           avg_turnaround,
           avg_waittime,
           throughput);
}

int DES::myRandom(int burst) {
    return 1 + (myRandom() % burst);
}

int DES::myRandom() {
    std::string line;
    getline(rand_file, line);

    if (rand_file.eof()) {
        rand_file.clear();
        rand_file.seekg(0);
        getline(rand_file, line);
        assert(line == "40000");
    }
    return std::stoi(line);
}


int DES::calCPUUtilTime() {
    int time = 0;
    for (auto s: CPUUtil) {
        time += (s.second - s.first);
    }
    return time;
}

int DES::calIOUtilTime() {
    // Klee's algorithm
    int n = IOUtil.size();
    std::vector<std::pair<int, bool>> points(n * 2);
    // Create a vector to store starting and ending points
    for (int i = 0; i < n; i++) {
        points[i * 2] = {IOUtil.at(i).first, false};
        points[i * 2 + 1] = {IOUtil.at(i).second, true};
    }
    // Sorting all points by point value
    std::sort(points.begin(), points.end());
    int result = 0; // Initialize result

    // To keep track of counts of current open segments (Starting point is processed, but ending point is not)
    int Counter = 0;
    // Trvaerse through all points
    for (int i = 0; i < n * 2; i++) {
        // If there are open points, then we add the difference between previous and current point.
        // This is interesting as we don't check whether current point is opening or closing.
        if (Counter)
            result += (points[i].first - points[i - 1].first);
        // If this is an ending point, reduce, count of open points.
        (points[i].second) ? Counter-- : Counter++;
    }
    return result;
}
