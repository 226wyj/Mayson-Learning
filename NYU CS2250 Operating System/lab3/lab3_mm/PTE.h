#ifndef LAB3_PTE_H
#define LAB3_PTE_H


#include "Frame.h"

struct PTE {
private:
    /* Pagedout: 1bit
     * Referenced: 1 bit
     * Modified: 1 bit
     * Write protect: 1 bit
     * Present/Absent: 1 bit
     * Page frame number: 7 bits
     */
    unsigned pagedout: 1;
    unsigned referenced: 1;
    unsigned modified: 1;
    unsigned write_protect: 1;
    unsigned present: 1;
    unsigned page_frame_number: 8;

    unsigned wp_checked:1;
    unsigned filemapped:1;
    unsigned fm_checked:1;
    unsigned unused: 15;
public:
    void set_pagedout() { pagedout = 1; }
    void clear_pagedout() { pagedout = 0; }
    bool is_pagedout() const { return pagedout; }

    void set_referenced() { referenced = 1; }
    void clear_referenced() { referenced = 0; }
    bool is_referenced() const { return referenced; }

    void set_modified() { modified = 1; }
    void clear_modified() { modified = 0; }
    bool is_modified() const { return modified; }

    void set_write_protect() { write_protect = 1; }
    // void clear_write_protect() { write_protect = 0; }
    bool is_write_protect() const { return write_protect; }

    void set_present() { present = 1; }
    void clear_present() { present = 0; }
    bool is_present() const { return present; };

    void instantiate(Frame *frame) { page_frame_number = unsigned(frame->id); };
    unsigned int get_frame_number() const { return page_frame_number; }

    void set_wp_checked() { wp_checked = 1; }
    bool is_wp_checked() const { return wp_checked; }

    void set_fm_checked() { fm_checked = 1; }
    bool is_fm_checked() const { return fm_checked; }

    void set_filemapped() { filemapped = 1; }
    bool is_filemapped() const { return filemapped; }
};


#endif //LAB3_PTE_H
