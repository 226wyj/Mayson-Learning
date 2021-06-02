# include "Simulator.h"
# include <sstream>

void Simulator::simulation() {
    Request *new_req = nullptr;
    Request *current_req = nullptr;
    bool finished = false;
    while (true) {
        /*
         * 1) If a new I/O arrived to the system at this current time -> add request to IO-queue ?
         * 2) If an IO is active and completed at this time -> Compute relevant info and store in IO request for final summary
         * 3) If an IO is active but did not yet complete -> Move the head by one sector/track/unit in the direction it is going (to simulate seek)
         * 4) If no IO request active now (after (2)) but IO requests are pending -> Fetch the next request and start the new IO.
         * 5) If no IO request is active now and no IO requests pending -> exit simulation
         * 6) Increment time by 1
         * */
        // If a new I/O arrived to the system at this current time -> add request to IO-queue
        if (req_index < IORequests.size()) {
            new_req = IORequests.at(req_index);
            if (new_req->arrival_time == current_time) {
                SCHEDULER->add_request(new_req);
                req_index++;
            }
        }

        // If an IO is active
        if (current_req) {
            // completed at this time -> Compute relevant info and store in IO request for final summary
            if (current_track == current_req->track) {
                current_req->end(current_time);
                current_req = nullptr;
                finished = true;
            }
                //  did not yet complete -> Move the head by one track in the direction it is going (to simulate seek)
            else {
                current_direction = current_req->track < current_track ? -1 : 1;
                current_track += current_direction;
                summary.tot_movement += 1;
            }
        }

        // If no IO request active now
        if (!current_req) {
            // IO requests are pending -> Fetch the next request and start the new IO.
            if (SCHEDULER->pending()) {
                current_req = SCHEDULER->select_request(current_track, current_direction);
                if (!current_req->started()) {
                    current_req->start(current_time);
                    continue;
                }
                if (finished) {
                    finished = false;
                    continue;
                }
            }
                // If no IO request is active now and no IO requests pending -> exit simulation
            else if (req_index >= IORequests.size()) {
                break;
            }
        }

        // Increment time by 1
        current_time += 1;
    }
}

void Simulator::report() {
    int waittime = 0;
    int turnaround = 0;
    int tot_turnaround = 0;
    int tot_waittime = 0;
    for (auto *req: IORequests) {
        printf("%5d: %5d %5d %5d\n", req->op_number, req->arrival_time, req->start_time, req->end_time);
        waittime = req->start_time - req->arrival_time;
        turnaround = req->end_time - req->arrival_time;
        summary.max_waittime = std::max(waittime, summary.max_waittime);
        tot_waittime += waittime;
        tot_turnaround += turnaround;
    }
    summary.total_time = current_time;
    summary.avg_waittime = (double) tot_waittime / IORequests.size();
    summary.avg_turnaround = (double) tot_turnaround / IORequests.size();


    printf("SUM: %d %d %.2lf %.2lf %d\n", summary.total_time, summary.tot_movement, summary.avg_turnaround,
           summary.avg_waittime, summary.max_waittime);
}

Simulator::Simulator(char algo, const std::string &inf_name) {
    switch (algo) {
        case 'i':
            SCHEDULER = new FIFO();
            break;
        case 'j':
            SCHEDULER = new SSTF();
            break;
        case 's':
            SCHEDULER = new LOOK();
            break;
        case 'c':
            SCHEDULER = new CLOOK();
            break;
        case 'f':
            SCHEDULER = new FLOOK();
            break;
        default:
            exit(-1);
    }
    input_file.open(inf_name);
    initialization();

}

void Simulator::initialization() {
    std::string line;
    int counter = 0;
    int time;
    int track;
    while (input_file) {
        getline(input_file, line);
        if (line.empty() || line[0] == '#') continue;
        std::stringstream s(line);
        s >> time >> track;
        auto *req = new Request(counter++, time, track);
        IORequests.emplace_back(req);
    }
}
