#ifndef LAB4_IOSCHEDULER_H
#define LAB4_IOSCHEDULER_H

# include <queue>
# include <vector>
# include "Request.h"

class IOScheduler {
public:
    virtual void add_request(Request *req) = 0;

    virtual Request *select_request(int, int) = 0;

    virtual bool pending() = 0;
};

class FIFO : public IOScheduler {
public:
    std::queue<Request *> io_queue;
    void add_request(Request *req) override;
    Request *select_request(int, int) override;
    bool pending() override;
};

class SSTF : public IOScheduler {
public:
    std::vector<Request *> io_queue;
    void add_request(Request *req) override;
    Request *select_request(int, int) override;
    bool pending() override;
};

class LOOK : public SSTF {
public:
    Request *select_request(int, int) override;
};

class CLOOK : public LOOK {
public:
    Request *select_request(int, int) override;
};

//class FLOOK : public IOScheduler {
//    std::vector<Request *> active_queue;
//    std::vector<Request *> add_queue;
//    void add_request(Request *req) override;
//    Request *select_request(int, int) override;
//    bool pending() override;
//
//};

class FLOOK : public LOOK {
public:
    std::vector<Request *> add_queue;
    void add_request(Request *req) override;
    Request *select_request(int, int) override;
    bool pending() override;

};


#endif //LAB4_IOSCHEDULER_H
