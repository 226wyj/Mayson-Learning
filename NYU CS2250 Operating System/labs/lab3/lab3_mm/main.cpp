# include <string>
# include <getopt.h>

# include "main.h"
# include "VirtualMemoryManager.h"

int main(int argc, char *argv[]) {
    std::string inf_name = argv[argc - 2];
    std::string rf_name = argv[argc - 1];

    int num_frames;
    char algo;
    std::string opt_string;

    bool Oflag = false; // creates output for operations
    bool Pflag = false; // final page table content
    bool Fflag = false; // final frame table content
    bool Sflag = false; // summary line
    bool xflag = false; // prints the current page table after each instructions


    // Argument Parser
    int c;
    while ((c = getopt(argc, argv, "f:a:o:")) != -1) {
        switch (c) {
            case 'f':
                num_frames = std::stoi(optarg);
                break;
            case 'a':
                algo = optarg[0];
                break;
            case 'o': //-oOPFS
                opt_string = optarg;
                if (opt_string.find('O') != std::string::npos) Oflag = true;
                if (opt_string.find('P') != std::string::npos) Pflag = true;
                if (opt_string.find('F') != std::string::npos) Fflag = true;
                if (opt_string.find('S') != std::string::npos) Sflag = true;
                if (opt_string.find('x') != std::string::npos) xflag = true;
                break;
            default:
                return 1;

        }
    }

    VirtualMemoryManager vmm(num_frames, algo, inf_name, rf_name, {Oflag, Pflag, Fflag, Sflag, xflag});
    vmm.simulation();
    vmm.report();
    return 0;
}
