//
// Created by mandriy on 5/25/15.
//

#ifndef TRANSLATORDESIGN_SEMANTICCHECKER_H
#define TRANSLATORDESIGN_SEMANTICCHECKER_H


#include "SyntaxTree.h"

#include <unordered_set>

class SemanticChecker {
public:

    void check(SyntaxTree &tree);

private:

    void reportError(string msg, Position pos);

    void checkVariableType(SyntaxTree &variableSubTree);

    void checkVariables(SyntaxTree &tree);

    vector<shared_ptr<TranslatorError>> errors;

};

#endif //TRANSLATORDESIGN_SEMANTICCHECKER_H
