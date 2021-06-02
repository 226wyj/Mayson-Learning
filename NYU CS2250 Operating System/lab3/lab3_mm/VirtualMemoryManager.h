#ifndef LAB3_VIRTUALMEMORYMANAGER_H
#define LAB3_VIRTUALMEMORYMANAGER_H

# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <queue>
# include "Process.h"
# include "Instruction.h"
# include "Pager.h"
# include "Frame.h"

class VirtualMemoryManager {
    const int MAX_FRAMES;
    const int MAX_VPAGES = 64;
    Pager *PAGER;

    const struct PrintOptions {
        bool O; // creates output for operations
        bool P; // final page table content
        bool F; // final frame table content
        bool S; // summary line
        bool x;
    } print_options;
    const struct OperationCost {
        const int read_write = 1;
        const int context_switch = 130;
        const int process_exit = 1250;
        const int maps = 300;
        const int unmaps = 400;
        const int ins = 3100;
        const int outs = 2700;
        const int fins = 2800;
        const int fouts = 2400;
        const int zeros = 140;
        const int segv = 340;
        const int segprot = 420;
    } COST;
    struct Summary {
        unsigned long inst_count = 0;
        unsigned long ctx_switches = 0;
        unsigned long process_exits = 0;
        unsigned long long cost = 0;
    } summary;

    std::ifstream input_file;
//    std::ifstream rand_file;

    Process *current_process = nullptr;
    std::vector<Process *> processes;

    std::vector<Frame> frame_table;
    std::queue<int> free_pool;

public:
    explicit VirtualMemoryManager(int, char, const std::string &, const std::string &, PrintOptions);
    ~VirtualMemoryManager();

    void simulation();
    void report();

private:
    Instruction *get_next_instruction();
    Frame *get_frame();
    Frame *allocate_frame_from_free_list();

    void init_process();
    void print_frame_table();

};


#endif //LAB3_VIRTUALMEMORYMANAGER_H
