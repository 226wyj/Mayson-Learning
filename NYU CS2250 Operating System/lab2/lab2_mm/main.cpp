#include <iostream>
#include <getopt.h>
#include "DES.h"


int main(int argc, char *argv[]) {
    std::string input_file = argv[argc - 2];
    std::string rand_file = argv[argc - 1];

    int c;
    int vflag = 0;
    int tflag = 0;
    int eflag = 0;

    std::string schedSpec;
    char scheduler;
    int quantum = 10000;
    int maxPrio = 4;

    // Argument Parser
    while ((c = getopt(argc, argv, "vtes:")) != -1) {
        switch (c) {
            case 'v': // verbose
                vflag = 1;
                break;
            case 't':
                tflag = 1;
                break;
            case 'e':
                eflag = 1;
                break;
            case 's':
                schedSpec = optarg;
                scheduler = schedSpec.at(0);
                switch (scheduler) {
                    case 'R': {
                        quantum = stoi(schedSpec.substr(1));
                        break;
                    }
                    case 'P':
                    case 'E': {
                        auto pos = schedSpec.find(':');
                        if (pos == std::string::npos) {
                            quantum = stoi(schedSpec.substr(1));
                        } else {
                            quantum = stoi(schedSpec.substr(1, pos));
                            maxPrio = stoi(schedSpec.substr(pos + 1));
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                return 1;
        }
    }

    DES des(input_file, rand_file, maxPrio, scheduler, quantum, vflag);
    des.initialization();
    des.simulation();
    des.report();

    return 0;
}