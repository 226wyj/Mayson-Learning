#include "VirtualMemoryManager.h"

void VirtualMemoryManager::simulation() {
    Instruction *next_instr = nullptr;
    while ((next_instr = get_next_instruction()) != nullptr) {
        printf("%lu: ==> %c %d\n", summary.inst_count++, next_instr->operation, next_instr->operand);

        if (next_instr->operation == 'c') { // context switch

            current_process = processes.at(next_instr->operand);
            summary.ctx_switches++;
            continue;
        } else if (next_instr->operation == 'e') { // process exit
            /* On process exit (instruction), you have to traverse the active process’s page table starting from 0..63
             * and for each valid entry UNMAP the page and FOUT modified filemapped pages.
             * Note that dirty non-fmapped (anonymous) pages are not written back (OUT) as the process exits.
             * The used frame has to be returned to the free pool and made available to the get_frame() function again.
             * The frames then should be used again in the order they were released.*/
            if (print_options.O) printf("EXIT current process %d\n", next_instr->operand);
            Process *proc_to_exit = processes.at(next_instr->operand);
            PTE *pte = nullptr;
            for (int i = 0; i < MAX_VPAGES; i++) {
                pte = &proc_to_exit->page_table.at(i);
                pte->clear_pagedout();
                if (pte->is_present()) {
                    pte->clear_present();
                    int frame_number = int(pte->get_frame_number());
                    if (print_options.O) printf(" UNMAP %d:%d\n", next_instr->operand, i);
                    proc_to_exit->states.unmaps++;
                    if (pte->is_modified() && proc_to_exit->is_filemapped(i)) {
                        if (print_options.O) printf(" FOUT\n");
                        proc_to_exit->states.fouts++;
                    }
                    frame_table.at(frame_number).clear();
                    free_pool.push(frame_number);
                }
            }
            summary.process_exits++;
            continue;
        }

        // next_instr->operation == 'w' or 'r'
        summary.cost += COST.read_write;

        int vpage = next_instr->operand;
        PTE *pte = &current_process->page_table[vpage]; // in reality this is done by hardware
        if (!pte->is_present()) { // in reality generates the page fault exception
            // First determine that the vpage can be accessed, i.e. it is part of one of the VMAs
            if (!current_process->in_vmas(vpage)) {
                // If not, a SEGV output line must be created and you move on to the next instruction
                printf(" SEGV\n");
                current_process->states.segv++;
                continue;
            }

            // If it is part of a VMA then a frame must be allocated, assigned to the PTE belonging to the vpage of this instruction
            Frame *new_frame = get_frame();
            /* Populated with the proper content.
             * The population depends whether this page was previously paged out
             * in which case the page must be brought back from the swap space “IN” or “FIN”.
             * If the vpage was never swapped out and is not file mapped,
             * then by definition it still has a zero filled content and you issue the “ZERO” output. */
            if (current_process->is_filemapped(vpage)) {
                if (print_options.O) printf(" FIN\n");
                current_process->states.fins++;
            } else if (pte->is_pagedout()) {
                if (print_options.O) printf(" IN\n");
                current_process->states.ins++;
            } else {
                if (print_options.O) printf(" ZERO\n");
                current_process->states.zeros++;
            }

            /* The PTE must be reset (note once the PAGEDOUT flag is set it will never be reset
             * as it indicates there is content on the swap device)
             * and then the PTE’s frame must be set and the valid bit can be set. */
            pte->clear_referenced();
            pte->clear_modified();
            pte->set_present();

            pte->instantiate(new_frame);
            printf(" MAP %d\n", new_frame->id);
            current_process->states.maps++;
            PAGER->reset_counter(new_frame, summary.inst_count);

            new_frame->reverse_map(current_process->id, vpage);
        }

        /* Its now ready for use and instruction
         * Simulate instruction execution by hardware by updating the R/M PTE bits based on operations */
        if (next_instr->operation == 'w') {
            /* Check write protection
            * In case the instruction is a write operation and the PTE’s write protect bit is set
            * (which it inherited from the VMA it belongs to) then a SEGPROT output line is to be generated.
            * The page is considered referenced but not modified in this case. */
            pte->set_referenced();
            if (current_process->is_write_protected(vpage)) {
                if (print_options.O) printf(" SEGPROT\n");
                current_process->states.segprot++;
            } else {
                pte->set_modified();
            }
        } else { // next_instr->operation == 'r'
            pte->set_referenced();
        }

        if (print_options.x) {
            current_process->print_pagetable();
        }
        delete next_instr;
        next_instr = nullptr;
    }


}

Instruction *VirtualMemoryManager::get_next_instruction() {
    std::string line;
    while (input_file) {
        getline(input_file, line);
        if (line.empty() || line[0] == '#')
            continue;
        auto *instr = new Instruction();
        std::stringstream s(line);
        s >> instr->operation >> instr->operand;
        return instr;
    }
    return nullptr;
}

Frame *VirtualMemoryManager::get_frame() {
    Frame *frame = allocate_frame_from_free_list();
    if (frame == nullptr) {
        frame = PAGER->select_victim_frame();
        /* Unmap its current user ( <address space:vpage>), i.e. its entry in the owning process’s page_table must be removed (“UNMAP”),
         * However you must inspect the state of the R and M bits. If the page was modified,
         * then the page frame must be paged out to the swap device (“OUT”)
         * or in case it was file mapped written back to the file (“FOUT”).*/
        if (print_options.O) printf(" UNMAP %d:%d\n", frame->pid, frame->vpage);
        processes.at(frame->pid)->states.unmaps++;

        auto *pte = &processes.at(frame->pid)->page_table.at(frame->vpage);
        pte->clear_present();
        if (pte->is_modified()) {
            if (processes.at(frame->pid)->is_filemapped(frame->vpage)) {
                if (print_options.O) printf(" FOUT\n");
                processes.at(frame->pid)->states.fouts++;
            } else {
                if (print_options.O) printf(" OUT\n");
                pte->set_pagedout();
                processes.at(frame->pid)->states.outs++;
            }
        }
        frame->clear();
    }
    return frame;
}

Frame *VirtualMemoryManager::allocate_frame_from_free_list() {
    if (free_pool.empty()) {
        return nullptr;
    } else {
        Frame *frm = &frame_table.at(free_pool.front());
        free_pool.pop();
        return frm;
    }
}

void VirtualMemoryManager::report() {
    if (print_options.P) {
        for (auto proc: processes) {
            proc->print_pagetable();
        }
    }
    if (print_options.F) {
        print_frame_table();
    }

    // Per process output:
    for (auto proc: processes) {
        printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
               proc->id,
               proc->states.unmaps,
               proc->states.maps,
               proc->states.ins,
               proc->states.outs,
               proc->states.fins,
               proc->states.fouts,
               proc->states.zeros,
               proc->states.segv,
               proc->states.segprot);

    }

    // Summary output:
    if (print_options.S) {
        for (auto proc: processes) {
            summary.cost += proc->states.unmaps * COST.unmaps;
            summary.cost += proc->states.maps * COST.maps;
            summary.cost += proc->states.ins * COST.ins;
            summary.cost += proc->states.outs * COST.outs;
            summary.cost += proc->states.fins * COST.fins;
            summary.cost += proc->states.fouts * COST.fouts;
            summary.cost += proc->states.zeros * COST.zeros;
            summary.cost += proc->states.segv * COST.segv;
            summary.cost += proc->states.segprot * COST.segprot;
        }
        summary.cost += summary.ctx_switches * COST.context_switch;
        summary.cost += summary.process_exits * COST.process_exit;

        printf("TOTALCOST %lu %lu %lu %llu %lu\n",
               summary.inst_count,
               summary.ctx_switches,
               summary.process_exits,
               summary.cost,
               sizeof(PTE));
    }


//    for (auto proc: processes) {
//        std::cout << "PID: " << proc->id << std::endl;
//        proc->print_vmas();
//    }

}

VirtualMemoryManager::VirtualMemoryManager(int
                                           max_frames, char
                                           algo,
                                           const std::string &inf_name,
                                           const std::string &rf_name,
                                           PrintOptions print_opt)
        : MAX_FRAMES(max_frames), print_options(print_opt) {
    switch (algo) {
        case 'f':
            PAGER = new FIFO(frame_table);
            break;
        case 'r':
            PAGER = new Random(frame_table, rf_name);
            break;
        case 'c':
            PAGER = new Clock(frame_table, processes);
            break;
        case 'e':
            PAGER = new NRU(frame_table, processes, &summary.inst_count);
            break;
        case 'a':
            PAGER = new Aging(frame_table, processes);
            break;
        case 'w':
            PAGER = new WorkingSet(frame_table, processes, &summary.inst_count);
            break;
        default:
            exit(-1);
    }

    input_file.open(inf_name);
    for (int i = 0; i < MAX_FRAMES; ++i) {
        frame_table.emplace_back(Frame{i});
        free_pool.push(i);
    }
    init_process();
}

VirtualMemoryManager::~VirtualMemoryManager() {
    // delete PAGER;
    input_file.close();
}

void VirtualMemoryManager::init_process() {
    std::string line;
    int num_process = -1;
    int num_vmas = -1;
    int start_vpage, ending_vpage, wt_protected, f_mapped;

    while (input_file) {
        getline(input_file, line);
        if (line.empty() || line[0] == '#')
            continue;
        num_process = std::stoi(line);
        break;
    }
    for (int i = 0; i < num_process; i++) {
        auto *proc = new Process(i);
        PTE pte = {};
        proc->page_table = std::vector<PTE>(MAX_VPAGES, {}); // zero-initialization for PTE
        processes.emplace_back(proc);

        while (input_file) {
            getline(input_file, line);
            if (line.empty() || line[0] == '#') continue;
            num_vmas = std::stoi(line);
            break;
        }
        for (int j = 0; j < num_vmas; j++) {
            while (input_file) {
                getline(input_file, line);
                if (line.empty() || line[0] == '#') continue;
                std::stringstream s(line);
                s >> start_vpage >> ending_vpage >> wt_protected >> f_mapped;
                VMA vma{start_vpage, ending_vpage, wt_protected, f_mapped};
                proc->vmas.emplace_back(vma);
                break;
            }
        }

    }
}

void VirtualMemoryManager::print_frame_table() {
    std::cout << "FT: ";
    for (auto frame: frame_table) {
        if (frame.used()) printf("%d:%d ", frame.pid, frame.vpage);
        else printf("* ");
    }
    std::cout << std::endl;
}