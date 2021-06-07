#ifndef LAB3_PROCESS_H
#define LAB3_PROCESS_H

# include <vector>
# include <iostream>
# include <algorithm>
# include "PTE.h"

struct VMA {
    int starting_virtual_page;
    int ending_virtual_page; // the VMA has (end_vpage – start_vpage + 1) virtual pages
    int write_protected; // binary whether the VMA is write protected or not
    int filemapped; // binary to indicate whether the VMA is mapped to a file or not
};

class Process {
public:
    int id;
    struct ProcessStates {
        unsigned long unmaps;
        unsigned long maps;
        unsigned long ins;
        unsigned long outs;
        unsigned long fins;
        unsigned long fouts;
        unsigned long zeros;
        unsigned long segv;
        unsigned long segprot;
    } states;

    std::vector<PTE> page_table; // a per process array of fixed size=64 of PTE not PTE pointers!
    std::vector<VMA> vmas;

    explicit Process(int);
    void print_vmas();
    void print_pagetable();

    bool in_vmas(int vpage);
    bool is_filemapped(int vpage);
    bool is_write_protected(int vpage);


};


#endif //LAB3_PROCESS_H
