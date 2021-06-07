#include <iostream>
#include <cassert>
#include "Tokenizer.h"
#include "Parser.h"

using namespace std;

int main(int argc, char *argv[]) {
    assert(argc == 2); //The program should take a single command line argument for input file.
    std::string fileName = argv[1];
    auto *tokenizer(new Tokenizer(fileName));

//    while (!tokenizer->eof()) {
//        auto t = tokenizer->getToken();
//        cout << t.first << ": " << t.second.first << " " << t.second.second << endl;
//    }
//    auto finalPos = tokenizer->getFinalPos();
//    cout << finalPos.first << " " << finalPos.second << endl;

    auto *parser(new Parser(*tokenizer));
    if (parser->passOne()) {
        parser->passTwo();
    }

    delete tokenizer;
    delete parser;
    return 0;
}
