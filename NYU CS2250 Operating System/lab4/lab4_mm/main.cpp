# include <iostream>
# include <getopt.h>
# include "Simulator.h"

int main(int argc, char *argv[]) {
    std::string inf_name = argv[argc - 1];
    char algo;


    // Argument Parser
    int c;
    while ((c = getopt(argc, argv, "s:")) != -1) {
        switch (c) {
            case 's':
                algo = optarg[0];
                break;
            default:
                return -1;
        }
    }
    Simulator sim(algo, inf_name);
    sim.simulation();
    sim.report();
    return 0;


}
