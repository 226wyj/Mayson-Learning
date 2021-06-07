#ifndef LAB2_UTILS_H
#define LAB2_UTILS_H

#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>

class Utils {
public:
    template<typename Out>
    static void split(const std::string &s, char delim, Out result);
    static std::vector<std::string> split(const std::string &s, char delim);
};


#endif //LAB2_UTILS_H
