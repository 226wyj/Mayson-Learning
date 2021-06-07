#ifndef LAB3_FRAME_H
#define LAB3_FRAME_H

# include <utility>

struct Frame {
    int id;
    int pid = -1;
    int vpage = -1;

    explicit Frame(int i) : id(i) {};
    bool used() const { return pid != -1 && vpage != -1; }
    void reverse_map(int pid, int vpage) {
        this->pid = pid;
        this->vpage = vpage;
    }
    void clear() { pid = vpage = -1; }

    unsigned long counter = 0;
    void shift() { counter >>= 1; }
    void set_leading_bit() { counter = (counter | 0x80000000); }
    void reset_counter() { counter = 0; }
};


#endif //LAB3_FRAME_H
