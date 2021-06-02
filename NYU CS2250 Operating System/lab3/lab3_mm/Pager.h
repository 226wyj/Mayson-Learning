#ifndef LAB3_PAGER_H
#define LAB3_PAGER_H

# include <vector>
# include <fstream>
# include <string>
# include <utility>
# include "Frame.h"
# include "Process.h"

class Pager {
public:
    std::vector<Frame> &frame_table;

    virtual Frame *select_victim_frame() = 0; // virtual base class
    virtual void reset_counter(Frame *, unsigned long time) {};
    // virtual ~Pager() = 0;
protected:
    explicit Pager(std::vector<Frame> &ft);
    void advance(int &hand) const { hand = (hand + 1) % int(frame_table.size()); }
};

class FIFO : public Pager {
private:
    int hand = 0;
public:
    explicit FIFO(std::vector<Frame> &ft);
    Frame *select_victim_frame() final;
};

class Random : public Pager {
private:
    std::ifstream rand_file;
    int my_random(int size);
public:
    explicit Random(std::vector<Frame> &ft, const std::string &rf_name);
    ~Random() { rand_file.close(); }
    Frame *select_victim_frame() final;
};

class Clock : public Pager {
protected:
    int hand = 0;  // The hand points to the oldest uninspected page
    std::vector<Process *> &processes;
public:
    explicit Clock(std::vector<Frame> &ft, std::vector<Process *> &procs);
    Frame *select_victim_frame() override;
};

class NRU : public Clock {
private:
    unsigned long *p_current_time;
    unsigned long last_reset_time = 0;
    int RESET_INTERVAL = 50;
public:
    NRU(std::vector<Frame> &ft, std::vector<Process *> &procs, unsigned long *p_time);
    Frame *select_victim_frame() override;
};

class Aging : public Clock {
public:
    Aging(std::vector<Frame> &ft, std::vector<Process *> &procs);
    Frame *select_victim_frame() final;
    void reset_counter(Frame *f, unsigned long time) override { f->reset_counter(); };
};

class WorkingSet : public Clock {
private:
    unsigned long *p_current_time;
    unsigned long TAU = 49;
public:
    WorkingSet(std::vector<Frame> &ft, std::vector<Process *> &procs, unsigned long *p_time);
    Frame *select_victim_frame() final;
    void reset_counter(Frame *f, unsigned long time) override { f->counter = time; };
};


#endif //LAB3_PAGER_H
