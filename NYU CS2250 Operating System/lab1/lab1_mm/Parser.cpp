#include <regex>
#include <iostream>
#include "Parser.h"

Parser::Parser(Tokenizer &t) : tokenizer(t) {}

bool Parser::passOne() {
    try {
        int baseAddress = 0;
        int moduleCounter = 1;
        int totalInst = 0;
        std::map<std::string, int> localSymbolTable;
        std::vector<std::string> localSymbolOrder;
        while (!tokenizer.eof()) {
            int defCount = readInt();
            if (defCount > MAX_NUM_DEF) {
                throw ParseErrorException(4, tokenizer.getLastPos().first, tokenizer.getLastPos().second);
            }
            for (int i = 0; i < defCount; i++) {
                std::string symbol = readSym();
                int val = readInt();

//                if (std::count(localSymbolOrder.begin(), localSymbolOrder.end(), symbol) == 0) {
//                    localSymbolOrder.emplace_back(symbol);
//                }
                localSymbolOrder.emplace_back(symbol);
                if (globalSymbolTable.count(symbol) == 0 && localSymbolTable.count(symbol) == 0) {
                    localSymbolTable.insert({symbol, val});
                    symbolUsedTable.insert({symbol, {moduleCounter, false}});
                } else {
                    symbolErrorTable.insert(
                            {symbol, " Error: This variable is multiple times defined; first value used"});
                }
            }
//            for (int i = 0; i < defCount; i++) {
//                std::string symbol = readSym();
//                int val = readInt();
//                if (globalSymbolTable.count(symbol) == 0 && localSymbolTable.count(symbol) == 0) {
//                    localSymbolTable.insert({symbol, val});
//                    localSymbolOrder.emplace_back(symbol);
//                    symbolUsedTable.insert({symbol, {moduleCounter, false}});
//                } else {
//                    symbolErrorTable.insert(
//                            {symbol, " Error: This variable is multiple times defined; first value used"});
//                }
//            }
            int useCount = readInt();
            if (useCount > MAX_NUM_USE) {
                throw ParseErrorException(5, tokenizer.getLastPos().first, tokenizer.getLastPos().second);
            }
            for (int i = 0; i < useCount; i++) {
                std::string symbol = readSym();
            }
            int instCount = readInt();
            totalInst += instCount;
            if (totalInst > MEMORY_SIZE) {
                throw ParseErrorException(6, tokenizer.getLastPos().first, tokenizer.getLastPos().second);
            }
            for (int i = 0; i < instCount; i++) {
                std::string addressMode = readIAER();
                readInt();
            }
            for (auto &symbol: localSymbolOrder) {
                if (localSymbolTable.count(symbol) && !globalSymbolTable.count(symbol)) {
                    auto val = localSymbolTable[symbol];
                    globalSymbolOrder.emplace_back(symbol);
                    if (val < instCount) {
                        globalSymbolTable.insert({symbol, val + baseAddress});
                    } else {
                        globalSymbolTable.insert({symbol, baseAddress});
                        printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", moduleCounter,
                               symbol.c_str(), val, instCount - 1);
                    }
                }
                else {
                    auto val = globalSymbolTable[symbol];
                    if (val >= baseAddress + instCount) {
                        printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", moduleCounter,
                               symbol.c_str(), 0, instCount - 1);
                    }
                }

            }
//            for (auto &symbol: localSymbolOrder) {
//                auto val = localSymbolTable[symbol];
//                globalSymbolOrder.emplace_back(symbol);
//                if (val < instCount) {
//                    globalSymbolTable.insert({symbol, val + baseAddress});
//                } else {
//                    globalSymbolTable.insert({symbol, baseAddress});
//                    printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", moduleCounter,
//                           symbol.c_str(), val, instCount - 1);
//                }
//            }
            baseAddress += instCount;
            moduleCounter++;
            localSymbolOrder.clear();
            localSymbolTable.clear();
        }

        std::cout << "Symbol Table" << std::endl;
        for (auto &symbol: globalSymbolOrder) {
            auto val = globalSymbolTable[symbol];
            std::string errmsg = symbolErrorTable[symbol];
//            std::cout << symbol << "=" << val << " " << errmsg << std::endl;
            std::cout << symbol << "=" << val << errmsg << std::endl;
        }
//        for (auto &item: globalSymbolTable) {
//            std::string errmsg = symbolErrorTable[item.first];
//            std::cout << item.first << "=" << item.second << " " << errmsg << std::endl;
//        }
        std::cout << std::endl;
    } catch (ParseErrorException &parseErrorException) {
        parseErrorException.print();
        return false;
    }
    return true;
}

void Parser::passTwo() {
    tokenizer.reset();
    std::cout << "Memory Map" << std::endl;

    int baseAddress = 0;
    int instCounter = 0;
    int moduleCounter = 1;
    std::vector<std::string> useList;
    std::map<int, bool> actuallyUsedTable;
    while (!tokenizer.eof()) {
        int defCount = readInt();
        for (int i = 0; i < defCount; i++) {
            std::string symbol = readSym();
            int val = readInt();
        }

        int useCount = readInt();
        for (int i = 0; i < useCount; i++) {
            std::string symbol = readSym();
            useList.emplace_back(symbol);
            actuallyUsedTable.insert({i, false});
        }

        int instCount = readInt();
        for (int i = 0; i < instCount; i++) {
            std::string addressMode = readIAER();
            int operand = readInt();
            auto res = resolve(addressMode, operand, baseAddress, instCount, useList, actuallyUsedTable);
            if (!res.second.empty()) {
                printf("%03d: %04d %s\n", instCounter++, res.first, res.second.c_str());
            } else {
                printf("%03d: %04d\n", instCounter++, res.first);
            }
//            printf("%03d: %04d %s\n", instCounter++, res.first, res.second.c_str());
        }

        for (int i = 0; i < useCount; i++) {
            if (not actuallyUsedTable[i]) {
                printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n", moduleCounter,
                       useList.at(i).c_str());
            } else {
                symbolUsedTable[useList[i]].second = true;
            }
        }
        useList.clear();
        actuallyUsedTable.clear();
        baseAddress += instCount;
        moduleCounter++;
    }

    std::cout << std::endl;
//    for (auto &item: symbolUsedTable) {
//        if (not item.second.second) {
//            printf("Warning: Module %d: %s was defined but never used\n", item.second.first, item.first.c_str());
//        }
//    }
    for (auto &symbol: globalSymbolOrder) {
        auto val = symbolUsedTable[symbol];
        if (not val.second) {
            printf("Warning: Module %d: %s was defined but never used\n", val.first, symbol.c_str());
        }
    }

}

std::string Parser::readIAER() {
    if (tokenizer.eof()) {
        throw ParseErrorException(2, tokenizer.getFinalPos().first, tokenizer.getFinalPos().second);
    }

    auto t = tokenizer.getToken();
    if (isIAER(t.first)) {
        return t.first;
    } else {
        throw ParseErrorException(2, t.second.first, t.second.second);
    }
}

std::string Parser::readSym() {
    if (tokenizer.eof()) {
        throw ParseErrorException(1, tokenizer.getFinalPos().first, tokenizer.getFinalPos().second);
    }

    auto t = tokenizer.getToken();
    if (isSym(t.first)) {
        if (t.first.length() <= 16) {
            return t.first;
        } else {
            throw ParseErrorException(3, t.second.first, t.second.second);
        }
    } else {
        throw ParseErrorException(1, t.second.first, t.second.second);
    }
}

int Parser::readInt() {
    if (tokenizer.eof()) {
        throw ParseErrorException(0, tokenizer.getFinalPos().first, tokenizer.getFinalPos().second);
    }

    auto t = tokenizer.getToken();
    if (isInt(t.first)) {
        return std::stoi(t.first);
    } else {
        throw ParseErrorException(0, t.second.first, t.second.second);
    }
}

bool Parser::isInt(const std::string &t) {
    return std::regex_match(t, std::regex("[0-9]+"));
}

bool Parser::isSym(const std::string &t) {
    return std::regex_match(t, std::regex("[A-Za-z][A-Za-z0-9]*"));
}

bool Parser::isIAER(const std::string &t) {
    return std::regex_match(t, std::regex("(I|A|E|R)"));
}

std::pair<int, std::string> Parser::resolve(const std::string &type,
                                            int instruction,
                                            int baseAddress,
                                            int moduleSize,
                                            const std::vector<std::string> &useList,
                                            std::map<int, bool> &usedTable) {
    int opCode = obtainOpCode(instruction);
    int oldOperand = obtainOperand(instruction);
    switch (type[0]) {
        case 'I':
            if (instruction > MAX_VAL) {
                return {MAX_VAL,
                        "Error: Illegal immediate value; treated as 9999"};
            } else {
                return {instruction, ""};
            }
        case 'A':
            if (opCode > MAX_OP_CODE) {
                return {MAX_VAL,
                        "Error: Illegal opcode; treated as 9999"};
            }
            if (oldOperand >= MEMORY_SIZE) {
                return {makeInstruction(opCode, 0),
                        "Error: Absolute address exceeds machine size; zero used"};
            } else {
                return {instruction, ""};
            }
        case 'E':
            if (opCode > MAX_OP_CODE) {
                return {MAX_VAL,
                        "Error: Illegal opcode; treated as 9999"};
            }
            if (oldOperand >= useList.size()) {
                return {instruction,
                        "Error: External address exceeds length of uselist; treated as immediate"};
            } else {
                std::string sym = useList.at(oldOperand);
                usedTable[oldOperand] = true;
                if (globalSymbolTable.count(sym)) {
                    return {makeInstruction(opCode, globalSymbolTable[useList.at(oldOperand)]), ""};
                } else {
                    return {makeInstruction(opCode, 0), "Error: " + sym + " is not defined; zero used"};
                }
            }
        case 'R':
            if (opCode > MAX_OP_CODE) {
                return {MAX_VAL,
                        "Error: Illegal opcode; treated as 9999"};
            }
            if (oldOperand >= moduleSize) {
                return {makeInstruction(opCode, baseAddress),
                        "Error: Relative address exceeds module size; zero used"};
            } else {
                return {makeInstruction(opCode, oldOperand + baseAddress), ""};
            }
        default:
            exit(-1);
    }
}

int Parser::obtainOpCode(int instruction) {
    return instruction / 1000;
}

int Parser::obtainOperand(int instruction) {
    return instruction % 1000;
}

int Parser::makeInstruction(int opCode, int operand) {
    return 1000 * opCode + operand;
}

Parser::ParseErrorException::ParseErrorException(int errorCode, int lineNum, int lineOffset) {
    this->errorCode = errorCode;
    this->lineNum = lineNum;
    this->lineOffset = lineOffset;
}

void Parser::ParseErrorException::print() {
    printf("Parse Error line %d offset %d: %s\n", lineNum, lineOffset, errStr[errorCode].c_str());
}
