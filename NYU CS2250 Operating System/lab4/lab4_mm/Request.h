#ifndef LAB4_REQUEST_H
#define LAB4_REQUEST_H


class Request {
public:
    int op_number;
    int arrival_time; // its arrival to the system (same as inputfile)
    int start_time; // its disk service start time
    int end_time; // its disk service end time
    int track;

    Request(int, int, int);
    void end(int);
    void start(int);
    bool started();
};


#endif //LAB4_REQUEST_H
