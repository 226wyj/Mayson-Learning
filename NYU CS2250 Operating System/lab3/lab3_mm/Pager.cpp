#include "Pager.h"

Pager::Pager(std::vector<Frame> &ft) : frame_table(ft) {}

FIFO::FIFO(std::vector<Frame> &ft) : Pager(ft) {}

Frame *FIFO::select_victim_frame() {
    auto *victim_frame = &frame_table.at(hand);
    advance(hand);
    return victim_frame;
}

Frame *Random::select_victim_frame() {
    auto *victim_frame = &frame_table.at(my_random(frame_table.size()));
    return victim_frame;
}

Random::Random(std::vector<Frame> &ft, const std::string &rf_name) : Pager(ft) {
    rand_file.open(rf_name);
    my_random(INT_MAX);
}

int Random::my_random(int size) {
    std::string line;
    getline(rand_file, line);
    if (rand_file.eof()) {
        rand_file.clear();
        rand_file.seekg(0);
        getline(rand_file, line);
        // std::stoi(line);
    }
    return std::stoi(line) % size;
}

Frame *Clock::select_victim_frame() {
    Frame *victim_frame;
    PTE *pte;
    while (true) {
        victim_frame = &frame_table.at(hand);
        advance(hand);
        pte = &processes.at(victim_frame->pid)->page_table.at(victim_frame->vpage);
        if (pte->is_referenced()) pte->clear_referenced();
        else return victim_frame;
    }
}

Clock::Clock(std::vector<Frame> &ft, std::vector<Process *> &procs) : Pager(ft), processes(procs) {}

Frame *NRU::select_victim_frame() {
    /* Requires that the REFERENCED-bit be periodically reset for all valid page table entries.
     * We simulate the periodic time inside the select_victim_frame function.
     * If 50 or more instructions have passed since the last time the reference bits were reset,
     * then the reference bit of the pte’s reached by the frame traversal should be cleared after
     * you considered the class of that frame/page.
     * Also in the algorithm you only have to remember the first frame that falls into each class
     * as it would be the one picked for that class. Naturally, when a frame for class-0 (R=0,M=0) is encountered,
     * you should stop the scan, unless the reference bits need to be reset, at which point you continue to
     * scan all frames. Once a victim frame is determined, the hand is set to the next position after the victim frame
     * for the next select_victim_frame() invocation. Try to walk the frametable only once using Booleans.*/
    Frame *victim_frame;
    PTE *pte;
    unsigned int class_index;
    std::vector<Frame *> candidates(4, nullptr);
    bool reset_flag = false;
    if (*p_current_time - last_reset_time >= RESET_INTERVAL) {
        last_reset_time = *p_current_time;
        reset_flag = true;
    }

    int frame_idx = hand;
    do {
        victim_frame = &frame_table.at(frame_idx);
        pte = &processes.at(victim_frame->pid)->page_table.at(victim_frame->vpage);
        class_index = 2 * pte->is_referenced() + pte->is_modified();
        if (reset_flag) pte->clear_referenced();
        if (candidates.at(class_index) == nullptr) {
            candidates.at(class_index) = victim_frame;
            if (class_index == 0 && !reset_flag) break;
        }
        advance(frame_idx);
    } while (frame_idx != hand);
    victim_frame = *std::find_if_not(candidates.begin(), candidates.end(), [](Frame *f) { return f == nullptr; });
    hand = (victim_frame->id + 1) % int(frame_table.size());
    return victim_frame;
}

NRU::NRU(std::vector<Frame> &ft, std::vector<Process *> &procs, unsigned long *p_time) : Clock(ft, procs),
                                                                                         p_current_time(p_time) {}

Frame *Aging::select_victim_frame() {
    /* AGING requires to maintain the age-bit-vector.
     * In this assignment please assume a 32-bit unsigned counter treated as a bit vector.
     * Note: “age = age >> 1” shifts the age to the right and “age = (age | 0x80000000)” sets the leading bit to one.
     * Aging is implemented on every page replacement request.
     * Since only active pages can have an age, it is fine to stick the age into the frame table entry.
     * Once the victim frame is determined, the hand is set to the next position after the victim frame
     * for the next select_victim invocation.
     * Note the age has to be reset to 0 on each MAP operation. Use a virtual pager function at the top to reset its value,
     * where others algorithms simply implement a default noop for that function.*/
    Frame *victim_frame = &frame_table.at(hand);
    Frame *candidate_frame;
    PTE *pte;

    int frame_idx = hand;
    do {
        candidate_frame = &frame_table.at(frame_idx);
        pte = &processes.at(candidate_frame->pid)->page_table.at(candidate_frame->vpage);
        candidate_frame->shift();
        if (pte->is_referenced()) candidate_frame->set_leading_bit();
        pte->clear_referenced();
        if (candidate_frame->counter < victim_frame->counter) victim_frame = candidate_frame;
        advance(frame_idx);
    } while (frame_idx != hand);
    hand = (victim_frame->id + 1) % int(frame_table.size());
    return victim_frame;
}

Aging::Aging(std::vector<Frame> &ft, std::vector<Process *> &procs) : Clock(ft, procs) {}

Frame *WorkingSet::select_victim_frame() {
    /* In the case of working set we also need to deal with time.
     * Again we use the execution of a single instruction.
     * We assume TAU=49, so if 50 or more instructions have passed since the time of “last use” was recorded
     * in the frame and the reference bit is not set, then this frame will be selected (see algo).
     * Note when you map a frame, you must set its time of last use to the current time (instruction count).
     * ! If no frame matches the time condition the one with the oldest “time_last_used” will be selected.
     * */
    Frame *victim_frame = &frame_table.at(hand);
    Frame *candidate_frame;
    PTE *pte;

    int frame_idx = hand;
    do {
        candidate_frame = &frame_table.at(frame_idx);
        pte = &processes.at(candidate_frame->pid)->page_table.at(candidate_frame->vpage);
        if (pte->is_referenced()) {
            candidate_frame->counter = *p_current_time;
            pte->clear_referenced();
        } else if (*p_current_time - candidate_frame->counter > TAU) {
            victim_frame = candidate_frame;
            break;
        } else if (candidate_frame->counter < victim_frame->counter) {
            victim_frame = candidate_frame;
        }
        advance(frame_idx);
    } while (frame_idx != hand);
    hand = (victim_frame->id + 1) % int(frame_table.size());
    return victim_frame;
}

WorkingSet::WorkingSet(std::vector<Frame> &ft, std::vector<Process *> &procs, unsigned long *p_time) :
        Clock(ft, procs), p_current_time(p_time) {}

