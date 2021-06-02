#include "Process.h"

Process::Process(int pid) : id(pid) {
    states = {};
}

void Process::print_vmas() {
    for (auto vma: vmas) {
        std::cout << vma.starting_virtual_page << ' '
                  << vma.ending_virtual_page << ' '
                  << vma.write_protected << ' '
                  << vma.filemapped << std::endl;
    }
}

void Process::print_pagetable() {
    printf("PT[%d]: ", id);
    for (int i = 0; i < page_table.size(); i++) {
        const auto &pte = page_table.at(i);
        if (in_vmas(i)) {
            if (pte.is_present()) {
                printf("%d:", i);
                printf(pte.is_referenced() ? "R" : "-");
                printf(pte.is_modified() ? "M" : "-");
                printf(pte.is_pagedout() ? "S " : "- ");
            } else {
                printf(pte.is_pagedout() ? "# " : "* ");
            }
        } else {
            printf("* ");
        }
    }
    printf("\n");
}

bool Process::in_vmas(int vpage) {
    return std::any_of(vmas.begin(), vmas.end(),
                       [vpage](const VMA &vma) {
                           return vpage >= vma.starting_virtual_page && vpage <= vma.ending_virtual_page;
                       });
}

bool Process::is_filemapped(int vpage) {
    if (page_table.at(vpage).is_fm_checked()) {
        return page_table.at(vpage).is_filemapped();
    } else {
        for (auto vma: vmas) {
            if (vpage >= vma.starting_virtual_page && vpage <= vma.ending_virtual_page) {
                page_table.at(vpage).set_fm_checked();
                if (vma.filemapped) page_table.at(vpage).set_filemapped();
                return is_filemapped(vpage);
            }
        }
        exit(-1);
    }
}

bool Process::is_write_protected(int vpage) {
    if (page_table.at(vpage).is_wp_checked()) {
        return page_table.at(vpage).is_write_protect();
    } else {
        for (auto vma: vmas) {
            if (vpage >= vma.starting_virtual_page && vpage <= vma.ending_virtual_page) {
                page_table.at(vpage).set_wp_checked();
                if (vma.write_protected) page_table.at(vpage).set_write_protect();
                return is_write_protected(vpage);
            }
        }
        exit(-1);
    }
}
