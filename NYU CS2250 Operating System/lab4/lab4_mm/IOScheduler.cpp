#include "IOScheduler.h"

void FIFO::add_request(Request *req) {
    io_queue.push(req);
}

Request *FIFO::select_request(int, int) {
    Request *req = nullptr;
    if (!io_queue.empty()) {
        req = io_queue.front();
        io_queue.pop();
    }
    return req;
}

bool FIFO::pending() {
    return !io_queue.empty();
}

void SSTF::add_request(Request *req) {
    io_queue.push_back(req);
}

Request *SSTF::select_request(int cur_track, int) {
    if (!pending()) {
        return nullptr;
    }

    Request *selected_req = io_queue.front();
    for (auto *req: io_queue) {
        if (std::abs(cur_track - req->track) < std::abs(cur_track - selected_req->track)) {
            selected_req = req;
        }
    }
    io_queue.erase(std::find(io_queue.begin(), io_queue.end(), selected_req));
    return selected_req;
}

bool SSTF::pending() {
    return !io_queue.empty();
}

Request *LOOK::select_request(int cur_track, int direction) {
    if (!pending()) {
        return nullptr;
    }
    Request *selected_req = nullptr;
    if (direction == 1) {
        for (auto *req: io_queue) {
            if (cur_track <= req->track) {
                if (selected_req == nullptr || req->track < selected_req->track) {
                    selected_req = req;
                }
            }
        }
    } else {
        for (auto *req: io_queue) {
            if (cur_track >= req->track) {
                if (selected_req == nullptr || req->track > selected_req->track) {
                    selected_req = req;
                }
            }
        }
    }
    if (selected_req == nullptr) {
        return select_request(cur_track, -direction);
    } else {
        io_queue.erase(std::find(io_queue.begin(), io_queue.end(), selected_req));
        return selected_req;
    }
}

Request *CLOOK::select_request(int cur_track, int) {
    if (!pending()) {
        return nullptr;
    }
    Request *selected_req = nullptr;
    Request *selected_req_rev = nullptr;
    for (auto *req: io_queue) {
        if (cur_track <= req->track) {
            if (selected_req == nullptr || req->track < selected_req->track) {
                selected_req = req;
            }
        } else {
            if (selected_req_rev == nullptr || req->track < selected_req_rev->track) {
                selected_req_rev = req;
            }
        }
    }
    selected_req = selected_req ? selected_req : selected_req_rev;
    io_queue.erase(std::find(io_queue.begin(), io_queue.end(), selected_req));
    return selected_req;
}

void FLOOK::add_request(Request *req) {
    add_queue.push_back(req);
}

Request *FLOOK::select_request(int cur_track, int direction) {
    if (!pending()) {
        return nullptr;
    }
    if (io_queue.empty()) {
        std::swap(io_queue, add_queue);
    }
    return LOOK::select_request(cur_track, direction);
}

bool FLOOK::pending() {
    return !io_queue.empty() || !add_queue.empty();
}
