//
// Created by mandriy on 5/25/15.
//

#include "../include/SemanticChecker.h"


void SemanticChecker::check(SyntaxTree &tree) {
    checkVariables(tree);
    if (errors.size()) {
        throw CompilerErrorsFound(errors);
    }
};

void SemanticChecker::reportError(string msg, Position pos) {
    errors.push_back(shared_ptr<TranslatorError>(new SemanticError(msg, pos)));
}

void SemanticChecker::checkVariableType(SyntaxTree &variableSubTree) {
    bool basicTypeSpecified = false;
    bool arrayAttrSpecified = false;
    variableSubTree.preTraverse(Matcher(Sort::Attribute, [&](shared_ptr<Node> node) -> void {
        if (((LeafNode *) node->getChildAt(0).get())->getToken()->code == Keyword::Keywords::Integer ||
            ((LeafNode *) node->getChildAt(0).get())->getToken()->code == Keyword::Keywords::Float) {
            if (basicTypeSpecified) {
                reportError("Basic type is already specified", node->getNodePosition());
            } else {
                basicTypeSpecified = true;
            }
        } else {
            if (arrayAttrSpecified) {
                reportError("Array attribute is already specified", node->getNodePosition());
            } else {
                int left = evalUnsignedInteger(node->getChildAt(1)->getChildAt(0));
                int right = evalUnsignedInteger(node->getChildAt(1)->getChildAt(2));
                if (left > right) {
                    reportError("Left border of an array should be not greater than right border", node->getNodePosition());
                }
                arrayAttrSpecified = true;
            }
        }
    }));
    if (!basicTypeSpecified) {
        reportError("Basic type should be specified", variableSubTree.getRoot()->getNodePosition());
    }
}

void SemanticChecker::checkVariables(SyntaxTree &tree) {
    vector<SyntaxTree::TraverseFunction> check;
    unordered_set<string> definedVars;
    string programName = "";
    check.push_back(Matcher(Sort::Program, [&](shared_ptr<Node> node) -> void {
        programName = getIdentifieString(node->getChildAt(1));
    }));
    check.push_back(Matcher(Sort::Declaration, [&](shared_ptr<Node> node) -> void {
        string varName = getIdentifieString(node->getChildAt(0));
        if (varName == programName) {
            reportError("Variable name conflicts with program name", node->getNodePosition());
        }
        if (definedVars.find(varName) == end(definedVars)) {
            definedVars.insert(varName);
            SyntaxTree variableSubTree(node);
            checkVariableType(variableSubTree);
        } else {
            reportError("Variable with such name is already defined", node->getNodePosition());
        }
    }));
    check.push_back(Matcher(Sort::Variable, [&](shared_ptr<Node> node) -> void {
        auto usedVarName = getIdentifieString(node->getChildAt(0));
        if (definedVars.find(usedVarName) == end(definedVars)) {
            reportError("Variable is not defined", node->getNodePosition());
        }
    }));
    tree.treeTraverse(check, vector<SyntaxTree::TraverseFunction>());
}