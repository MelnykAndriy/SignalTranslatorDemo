//
// Created by mandriy on 5/16/15.
//

#include <memory>
#include <iostream>

#include "../include/errors.h"
#include "../include/Lexer.h"

using namespace std;

vector<shared_ptr<Token> > Lexer::lexicalAnalysis(vector<string> lines) {
    this->prepareForAnalysis();

    unsigned int lineNumber = 1;
    bool insideComment = false;
    Position commentBeginPos(0, 0);

    for (string &line: lines) {
        auto ch = begin(line);
        auto beginOfLine = begin(line);
//        auto currentPos =
        auto hasNextChar = [&]() -> bool { return ch != end(line); };

        while (hasNextChar()) {

            if (hasNextChar() && *ch == '(') {
                commentBeginPos = Position(ch - beginOfLine, lineNumber);
                ch++;
                if (hasNextChar() && *ch == '*') {
                    insideComment = true;
                } else {
                    errors.push_back(shared_ptr<TranslatorError>(
                            new LexicalError("Invalid token", Position(ch - beginOfLine, lineNumber))));
                }
                ch++;
            }

            if (insideComment) {
                char prevCh = ' ';
                while (hasNextChar() && !(prevCh == '*' && *ch == ')')) {
                    prevCh = *ch;
                    ch++;
                }
                if (hasNextChar()) {
                    insideComment = false;
                    ch++;
                }
            }
            if (hasNextChar() && isspace(*ch)) {
                ch++;
                while (hasNextChar() && isspace(*ch)) {
                    ch++;
                }
                continue;
            }

            if (hasNextChar() && Delimiter::isDelimiter(string("") + *ch)) {
                processDelimiter(ch, hasNextChar, [=]() -> Position {
                    return Position((unsigned int) (ch - beginOfLine + 1), lineNumber);
                });
                continue;
            }

            if (hasNextChar() && isalpha(*ch) && isupper(*ch)) {
                processIdentifier(ch, hasNextChar, [=]() -> Position {
                    return Position((unsigned int) (ch - beginOfLine + 1), lineNumber);
                });
                continue;
            }

            if (hasNextChar() && isdigit(*ch)) {
                processNumber(ch, hasNextChar, [=]() -> Position {
                    return Position((unsigned int) (ch - beginOfLine + 1), lineNumber);
                });
                continue;
            }

            if (hasNextChar()) {
                errors.push_back(shared_ptr<TranslatorError>(
                        new LexicalError("Invalid token", Position(ch - beginOfLine, lineNumber))));
                ch++;
            }
        }
        lineNumber++;
    }
    if (insideComment) {
        errors.push_back(shared_ptr<TranslatorError>(new LexicalError("Comment was not completed.", commentBeginPos)));
    }

    if (!errors.empty()) {
        throw CompilerErrorsFound(errors);
    }

    return tokens;
}

void Lexer::processDelimiter(CharIterRef ch, function<bool()> hasNextChar, function<Position()> tokenPos) {
    if (Delimiter::isMultipleSymbolDelimiterBegin(*ch)) {
        char fistSymbol = *ch;
        string multipleSymbolDelimiter(1, *ch);
        ch++;
        while (hasNextChar() && isspace(*ch)) {
            ch++;
        }
        if (hasNextChar() && Delimiter::isDelimiter(multipleSymbolDelimiter += *ch)) {
            shared_ptr<Token> multipleDelim(
                    new Token(Delimiter::delimiterCode(multipleSymbolDelimiter), multipleSymbolDelimiter, tokenPos()));
            tokens.push_back(multipleDelim);
            ch++;
        } else {
            shared_ptr<Token> colon(
                    new Token(Delimiter::delimiterCode(string(1, fistSymbol)), string(1, fistSymbol), tokenPos()));
            tokens.push_back(colon);
        }
    } else {
        shared_ptr<Token> delimiter(new Token(Delimiter::delimiterCode(string("") + *ch), string("") + *ch,
                                              tokenPos()));
        tokens.push_back(delimiter);
        ch++;
    }
}

void Lexer::processIdentifier(CharIterRef ch, function<bool()> hasNextChar, function<Position()> tokenPos) {
    string identifier;
    while (hasNextChar() && (isdigit(*ch) || isalpha(*ch) && isupper(*ch))) {
        identifier += *ch;
        ch++;
    }
    if (!hasNextChar() || isspace(*ch) || Delimiter::isDelimiter(string(1, *ch))) {
        if (Keyword::isKeyword(identifier)) {
            shared_ptr<Token> keyword(new Token(Keyword::keywordCode(identifier), identifier, tokenPos()));
            tokens.push_back(keyword);
        } else {
            if (identifiersOccurence.find(identifier) != end(identifiersOccurence)) {
                tokens.push_back(shared_ptr<Token>(new Token(identifiersOccurence.at(identifier), identifier, tokenPos())));
            } else {
                shared_ptr<Token> identifierToken(new Token(identifierCodeCounter++, identifier, tokenPos()));
                identifiersTable[identifierToken->code] = identifierToken;
                tokens.push_back(identifierToken);
                identifiersOccurence[identifier] =  identifierToken->code;
            }
        }
    } else {
        while (hasNextChar() && !isspace(*ch) && !Delimiter::isDelimiter(string(1, *ch))) {
            ch++;
        }
        errors.push_back(
                shared_ptr<TranslatorError>(new LexicalError("Invalid token", tokenPos())));
    }
}

void Lexer::processNumber(CharIterRef ch, function<bool()> hasNextChar, function<Position()> tokenPos) {
    string number("");
    while (hasNextChar() && isdigit(*ch)) {
        number += *ch;
        ch++;
    }
    if (!hasNextChar() || isspace(*ch) || Delimiter::isDelimiter(string(1, *ch))) {
        if ( constantsOccurence.find(number) != end(constantsOccurence)) {
            tokens.push_back(shared_ptr<Token>(new Token(constantsOccurence.at(number), number, tokenPos())));
        } else {
            shared_ptr<Token> numberToken(
                    new Token(constantCodeCounter++, number, tokenPos()));
            constantsTable[numberToken->code] = numberToken;
            tokens.push_back(numberToken);
            constantsOccurence[number] = numberToken->code;
        }
    } else {
        while (hasNextChar() && !isspace(*ch) && !Delimiter::isDelimiter(string(1, *ch))) {
            ch++;
        }
        errors.push_back(shared_ptr<TranslatorError>(new LexicalError("Invalid token", tokenPos())));
    }
}

void Lexer::prepareForAnalysis() {
    identifierCodeCounter = 1001;
    constantCodeCounter = 501;
    identifiersTable.clear();
    constantsTable.clear();
    tokens.clear();
    errors.clear();
}

