//
// Created by mandriy on 5/16/15.
//

#ifndef TRANSLATORDESIGN_UTILS_H
#define TRANSLATORDESIGN_UTILS_H

#include <string>
#include <unordered_map>

bool isIdentifierCode(unsigned int code);
bool isConstantCode(unsigned int code);

class Keyword {
public:
    enum Keywords {
        Program = 401, Begin = 402, End = 403, Var = 404, Integer = 405, Float = 406, Loop = 407, Endloop = 408
    };

    static bool isKeyword(std::string str);
    static unsigned int keywordCode(std::string keyword);

private:

    static void initKeywords();
    static std::unordered_map<std::string, unsigned int> keywords;
};

class Delimiter {
public:
    enum MultipleSymbolDelimiters {
        Assigne = 301, Range = 302
    };

    enum Delimiters {
        Dot = '.', Colon = ':', Semicolon = ';', OpenBracket = '[', CloseBracket = ']'
    };

    static bool isDelimiter(std::string str);
    static unsigned int delimiterCode(std::string delim);
    static bool isMultipleSymbolDelimiterBegin(char &i);
private:
    static void initDelimiters();
    static std::unordered_map<std::string, unsigned int> delimiters;
};


struct Position {
    Position(unsigned int column, unsigned int line) : column(column), line(line) { }

    unsigned int column;
    unsigned int line;
};

struct Token {
    Token(unsigned int code, std::string rep, const Position pos) : code(code), rep(rep), pos(pos) { }

    unsigned int code;
    std::string rep;
    Position pos;
};



#endif //TRANSLATORDESIGN_UTILS_H
