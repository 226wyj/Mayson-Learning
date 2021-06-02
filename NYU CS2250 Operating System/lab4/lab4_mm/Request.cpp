#include "Request.h"

Request::Request(int op_num, int arr_t, int track) : op_number(op_num), arrival_time(arr_t), track(track) {
    start_time = -1;
    end_time = -1;
}

void Request::end(int time) {
    end_time = time;
}

void Request::start(int time) {
    start_time = time;
}

bool Request::started() {
    return start_time != -1;
}
