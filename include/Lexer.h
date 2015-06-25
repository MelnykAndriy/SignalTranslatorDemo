//
// Created by mandriy on 5/16/15.
//

#ifndef TRANSLATORDESIGN_LEXER_H
#define TRANSLATORDESIGN_LEXER_H


#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "utils.h"
#include "errors.h"


class Lexer {
public:

    Lexer() { }

    virtual ~Lexer() { }

    std::unordered_map<unsigned int, std::shared_ptr<Token> > const &getIdentifiersTable() const {
        return identifiersTable;
    }

    std::unordered_map<unsigned int, std::shared_ptr<Token> > const &getConstantsTable() const {
        return constantsTable;
    }


    std::vector<std::shared_ptr<Token> > lexicalAnalysis(std::vector<std::string> lines);

private:
    typedef std::basic_string<char>::iterator &CharIterRef;

    void prepareForAnalysis();

    void processNumber(CharIterRef ch, std::function<bool()> hasNextChar, std::function<Position()> tokenPos);

    void processIdentifier(CharIterRef ch, std::function<bool()> hasNextChar, std::function<Position()> tokenPos);

    void processDelimiter(CharIterRef ch, function<bool()> hasNextChar, function<Position()> tokenPos);


    std::unordered_map<unsigned int, std::shared_ptr<Token> > identifiersTable;
    std::unordered_map<unsigned int, std::shared_ptr<Token> > constantsTable;
    std::unordered_map<string, unsigned int> identifiersOccurence;
    std::unordered_map<string, unsigned int> constantsOccurence;

    unsigned int identifierCodeCounter;
    unsigned int constantCodeCounter;
    std::vector<std::shared_ptr<Token> > tokens;
    std::vector<shared_ptr<TranslatorError> > errors;


};


#endif //TRANSLATORDESIGN_LEXER_H
