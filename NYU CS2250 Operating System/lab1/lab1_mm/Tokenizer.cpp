#include "Tokenizer.h"

#include <utility>
#include <regex>
#include <cassert>

int Tokenizer::split(char *str, char *delimiter, std::vector<std::string> *tokens, std::vector<int> *positions) {
    int numTokens = 0;
    char *pch = strtok(str, delimiter);
    while (pch != nullptr) {
        tokens->push_back(pch);
        positions->push_back(pch - str + 1);
        numTokens++;
        pch = strtok(nullptr, delimiter);
    }
    return numTokens;
}


Tokenizer::Tokenizer(std::string fileName) : fileName(std::move(fileName)) {
    inputFile = std::ifstream(this->fileName);
    assert(inputFile);
}

bool Tokenizer::eof() {
    return inputFile.eof();
}

Tokenizer::~Tokenizer() {
    inputFile.close();
}

std::pair<std::string, std::pair<int, int>> Tokenizer::getToken() {
    if (bufferedTokenIndex < tokens.size()) {
        int curIndex = bufferedTokenIndex++;
        auto t = std::pair<std::string, std::pair<int, int>>(
                {tokens.at(curIndex), {lineNum, lineOffsets.at(curIndex)}});
        lastPos = t.second;
        if (bufferedTokenIndex == tokens.size()) {
            int curLineNum = lineNum;
            while (inputFile.peek() == '\n' or inputFile.peek() == '\t' or inputFile.peek() == ' ') {
                char c = inputFile.get();
                if (c == '\n') {
                    lineNum++;
                }
            }
            if (inputFile.peek() == EOF) {
                inputFile.get();
                if (curLineNum == lineNum) {
                    finalPos = {curLineNum, lineOffsets.back() + tokens.back().length()};
                } else {
                    finalPos = {lineNum, 1};
                }

            }
        }
        return t;

    } else {
        tokens.clear();
        lineOffsets.clear();
        bufferedTokenIndex = 0;
        std::string line;
        do {
            lineNum++;
            getline(inputFile, line);
        } while (Tokenizer::split(const_cast<char *>(line.data()), DELIMITERS, &tokens, &lineOffsets) <= 0 && !eof());
        if (eof() && tokens.empty()) {
            return std::pair<std::string, std::pair<int, int>>({"", {-1, -1}});
        }
        else {
            return getToken();
        }
//        return eof() ? std::pair<std::string, std::pair<int, int>>({"", {-1, -1}}) : getToken();
    }

}

const std::pair<int, int> &Tokenizer::getFinalPos() const {
    return finalPos;
}

const std::pair<int, int> &Tokenizer::getLastPos() const {
    return lastPos;
}

void Tokenizer::reset() {
    tokens.clear();
    lineOffsets.clear();
    bufferedTokenIndex = 0;
    lineNum = 0;
    lastPos = {-1, -1};
    finalPos = {-1, -1};
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);
}




