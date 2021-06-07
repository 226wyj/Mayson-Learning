#ifndef LAB4_SIMULATOR_H
#define LAB4_SIMULATOR_H

# include <fstream>
# include <cstdio>
# include <vector>
# include <queue>
# include "IOScheduler.h"
# include "Request.h"



class Simulator {
public:
    struct Summary {
        int total_time = 0; // total simulated time, i.e. until the last I/O request has completed
        int tot_movement = 0; // total number of tracks the head had to be moved
        double avg_turnaround = 0; // average turnaround time per operation from time of submission to time of completion
        double avg_waittime = 0; // average wait time per operation (time from submission to issue of IO request to start disk operation)
        int max_waittime = 0; // maximum wait time for any IO operation
    } summary;

    int current_time = 0;
    int current_track = 0;
    int current_direction = 1;
    int req_index = 0;
    std::ifstream input_file;
    std::vector<Request*> IORequests;
    IOScheduler* SCHEDULER;

    explicit Simulator(char, const std::string&);

    void initialization();

    void simulation();

    void report();
};


#endif //LAB4_SIMULATOR_H
