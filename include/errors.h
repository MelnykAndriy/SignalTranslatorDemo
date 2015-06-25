//
// Created by mandriy on 5/16/15.
//

#ifndef TRANSLATORDESIGN_ERRORS_H
#define TRANSLATORDESIGN_ERRORS_H

#include <exception>
#include <string>
#include <vector>
#include <sstream>
#include <memory>

#include "utils.h"

using namespace std;

class TranslatorError {
public:
    TranslatorError(string msg, Position pos) : msg(msg), pos(pos) { }

    friend ostream &operator<<(ostream &os, const TranslatorError &error) {
        os << error.errorType() << " " <<  error.msg << ". Line " <<
        error.pos.line << ", column " << error.pos.column;
        return os;
    }

protected:
    virtual string errorType() const = 0;
private:
    string msg;
    Position pos;

};

struct LexicalError : public TranslatorError {
public:
    LexicalError(string msg, Position pos) : TranslatorError(msg, pos) { }

protected:
    virtual string errorType() const { return "Lexical ERROR:"; }

};

class SyntaxError : public TranslatorError {
public:
    SyntaxError(string msg, Position pos) : TranslatorError(msg, pos) { }

protected:
    virtual string errorType() const { return "Syntax ERROR:"; }
};

class SemanticError : public TranslatorError {
public:
    SemanticError(string msg, Position pos) : TranslatorError(msg, pos) { }

protected:
    virtual string errorType() const { return "Semantic ERROR:"; }
};

class CompilerErrorsFound : public std::exception {
public:
    CompilerErrorsFound(vector<shared_ptr<TranslatorError> > errors) : errors(errors) {
        stringstream msg;
        msg << "Compiler errors found : " << errors.size();
    }

    virtual ~CompilerErrorsFound() { }

    virtual const char *what() const noexcept(true) override { return msg.c_str(); }

    const vector<shared_ptr<TranslatorError> > &getErrors() { return errors; }

private:
    const vector<shared_ptr<TranslatorError> > errors;
    string msg;
};

void dumpErrors(const vector<shared_ptr<TranslatorError> > &errors);


#endif //TRANSLATORDESIGN_ERRORS_H


