//
// Created by mandriy on 5/16/15.
//

#include "../include/utils.h"

using namespace std;

bool isIdentifierCode(unsigned int code) {
    return code > 1000;
}

bool isConstantCode(unsigned int code) {
    return code < 1001 && code > 500;
}


unordered_map<string, unsigned int> Keyword::keywords;
unordered_map<string, unsigned int> Delimiter::delimiters;

bool Keyword::isKeyword(std::string str) {
    if (keywords.empty()) {
        initKeywords();
    }
    return keywords.find(str) != end(keywords);
}

unsigned int Keyword::keywordCode(std::string keyword) {
    if (keywords.empty()) {
        initKeywords();
    }
    return keywords.at(keyword);
}

void Keyword::initKeywords() {
    keywords["BEGIN"] = Keywords::Begin;
    keywords["PROGRAM"] = Keywords::Program;
    keywords["END"] = Keywords::End;
    keywords["VAR"] = Keywords::Var;
    keywords["INTEGER"] = Keywords::Integer;
    keywords["FLOAT"] = Keywords::Float;
    keywords["LOOP"] = Keywords::Loop;
    keywords["ENDLOOP"] = Keywords::Endloop;
}

bool Delimiter::isDelimiter(std::string str) {
    initDelimiters();

    return delimiters.find(str) != end(delimiters);
}

unsigned int Delimiter::delimiterCode(std::string delim) {
    initDelimiters();
    return delimiters.at(delim);
}

bool Delimiter::isMultipleSymbolDelimiterBegin(char &i) {
    initDelimiters();
    return i == ':' || i == '.';
}

void Delimiter::initDelimiters() {
    if (delimiters.empty()) {
        delimiters[":"] = Delimiters::Colon;
        delimiters[";"] = Delimiters::Semicolon;
        delimiters["."] = Delimiters::Dot;
        delimiters["["] = Delimiters::OpenBracket;
        delimiters["]"] = Delimiters::CloseBracket;
        delimiters[":="] = MultipleSymbolDelimiters::Assigne;
        delimiters[".."] = MultipleSymbolDelimiters::Range;
    }
}


