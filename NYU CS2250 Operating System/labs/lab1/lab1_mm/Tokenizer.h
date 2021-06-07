#ifndef LAB1_TOKENIZER_H
#define LAB1_TOKENIZER_H

#include <string>
#include <vector>
#include <utility>
#include <fstream>

class Tokenizer {
public:
    char DELIMITERS[4] = " \t\n";

    explicit Tokenizer(std::string fileName);

    ~Tokenizer();

    bool eof();

    void reset();

    std::pair<std::string, std::pair<int, int>> getToken();

    const std::pair<int, int> &getFinalPos() const;

    const std::pair<int, int> &getLastPos() const;


private:
    std::string fileName;
    std::ifstream inputFile;

    int lineNum = 0;
    int bufferedTokenIndex = 0;
    std::vector<std::string> tokens;
    std::vector<int> lineOffsets;
    std::pair<int, int> finalPos = {-1, -1};
    std::pair<int, int> lastPos = {-1, -1};

    static int split(char *str, char *delimiter, std::vector<std::string> *tokens, std::vector<int> *positions);

};


#endif //LAB1_TOKENIZER_H
