#include "Utils.h"

template<typename Out>
void Utils::split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (getline(iss, item, delim)) {
        if (!item.empty()) {
            *result++ = item;
        }
    }
}

std::vector<std::string> Utils::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}