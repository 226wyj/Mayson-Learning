#ifndef LAB1_PARSER_H
#define LAB1_PARSER_H


#include <map>
#include "Tokenizer.h"

class Parser {
public:
    class ParseErrorException : public std::exception {
    public:
        const std::vector<std::string> errStr = {
                "NUM_EXPECTED", // Number expect
                "SYM_EXPECTED", // Symbol Expected
                "ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
                "SYM_TOO_LONG", // Symbol Name is too long
                "TOO_MANY_DEF_IN_MODULE", // > 16
                "TOO_MANY_USE_IN_MODULE", // > 16
                "TOO_MANY_INSTR" // total num_instr exceeds memory size (512)
        };

        ParseErrorException(int errorCode, int lineNum, int lineOffset);

        void print();

    private:
        int errorCode;
        int lineNum;
        int lineOffset;

    };

    const int MEMORY_SIZE = 512;
    const int MAX_VAL = 9999;
    const int MAX_NUM_DEF = 16;
    const int MAX_NUM_USE = 16;
    const int MAX_OP_CODE = 9;

    bool passOne();

    void passTwo();

private:
    Tokenizer &tokenizer;
    std::map<std::string, int> globalSymbolTable;
    std::vector<std::string> globalSymbolOrder;
    std::map<std::string, std::pair<int, bool>> symbolUsedTable;
    std::map<std::string, std::string> symbolErrorTable;


    int readInt();

    std::string readSym();

    std::string readIAER();

    static bool isInt(const std::string &t);

    static bool isSym(const std::string &t);

    static bool isIAER(const std::string &t);


public:
    explicit Parser(Tokenizer &tokenizer);

    static int obtainOpCode(int instruction);

    static int obtainOperand(int instruction);

    static int makeInstruction(int opCode, int operand);


    std::pair<int, std::string> resolve(const std::string &type, int instruction, int baseAddress, int moduleSize,
                                        const std::vector<std::string> &useList, std::map<int, bool> &usedTable);
};


#endif //LAB1_PARSER_H
