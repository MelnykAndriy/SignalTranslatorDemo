//
// Created by mandriy on 5/26/15.
//

#include "../include/AsmCodeGenerator.h"
#include <cstdlib>


ostream &endl(ostream &os) {
    IndentOstream *indentStream = dynamic_cast<IndentOstream *>(&os);
    if (indentStream) {
        indentStream->indentIsNeeded = true;
    }
    os << "\n";
    return os;
}

void IndentOstream::makeIndent() {
    for (auto i = 0; i != *indentCount; i++) {
        *this << indent;
    }
}


string AsmCodeGenerator::generateAsm(SyntaxTree &tree, unordered_map<unsigned int, std::shared_ptr<Token>> identifiers,
                                     unordered_map<unsigned int, std::shared_ptr<Token>> constants) {
    IndentOstream asmCode(indent, &indentDepth);
    vector<SyntaxTree::TraverseFunction> pre;
    vector<SyntaxTree::TraverseFunction> post;
    stack<string> loopLabels;

    names.clear();
    for (auto identifier: identifiers) {
        names.insert(identifier.second->rep);
    }
    pre.push_back(Matcher(Sort::Program, [&](shared_ptr<Node> program) -> void {
        auto programName = getIdentifieString(program->getChildAt(1));
        asmCode << programName << " PROC" << endl;
        indentDepth++;
    }));
    pre.push_back(Matcher(Sort::Statement, [&](shared_ptr<Node> statement) -> void {
        if (statement->getChildAt(0)->getLabel() == "LOOP") {
            loopLabels.push(generateUniqueName());
            asmCode << loopLabels.top() << ":" << endl;
            indentDepth++;
        } else if (((InteriorNode *) statement->getChildAt(0).get())->getSort() == Sort::Variable) {
            generateAssignment(statement, asmCode);
        }

    }));


    post.push_back(Matcher(Sort::Program, [&](shared_ptr<Node> program) -> void {
        auto programName = getIdentifieString(program->getChildAt(1));
        asmCode << "RET" << endl;
        indentDepth--;
        asmCode << programName << " ENDP" << endl;
    }));
    post.push_back(Matcher(Sort::Statement, [&](shared_ptr<Node> statement) -> void {
        if (statement->getChildAt(0)->getLabel() == "LOOP") {
            indentDepth--;
            asmCode << "JMP " << loopLabels.top() << endl;
            loopLabels.pop();
        }
    }));

    generateDataSegment(tree, asmCode);
    tree.treeTraverse(pre, post);

    return asmCode.str();
}


string AsmCodeGenerator::generateUniqueName() {
    while (true) {
        string uniqueName;
        for (auto i = 0; i < randomStringSize; i++) {
            uniqueName += (char) ((rand() % 26) + 65);
        }
        if (names.find(uniqueName) == end(names)) {
            names.insert(uniqueName);
            return uniqueName;
        }
    }
}

void AsmCodeGenerator::generateDataSegment(SyntaxTree &tree, IndentOstream &code) {
    code << "_DATA SEGMENT" << endl;
    indentDepth++;
    tree.preTraverse(Matcher(Sort::Declaration, [&](shared_ptr<Node> declaration) -> void {
        code << getIdentifieString(declaration->getChildAt(0)) << " ";
        generateVariableType(declaration, code);
        code << endl;
    }));
    indentDepth--;
    code << "_DATA ENDS" << endl;

}

void AsmCodeGenerator::generateVariableType(shared_ptr<Node> declaration, IndentOstream &code) {
    SyntaxTree decl(declaration);

    int arraySize = -1;
    int basicType = -1;

    decl.preTraverse(Matcher(Sort::Attribute, [&](shared_ptr<Node> attr) -> void {
        if (((LeafNode *) attr->getChildAt(0).get())->getToken()->code == Keyword::Keywords::Integer ||
            ((LeafNode *) attr->getChildAt(0).get())->getToken()->code == Keyword::Keywords::Float) {
            basicType = ((LeafNode *) attr->getChildAt(0).get())->getToken()->code;
        } else {
            arraySize = evalUnsignedInteger(attr->getChildAt(1)->getChildAt(2)) -
                        evalUnsignedInteger(attr->getChildAt(1)->getChildAt(0));
        }
    }));

    code << "DD ";
    string init;

    switch (basicType) {
        case Keyword::Keywords::Integer:
            init = "0";
            break;
        case Keyword::Keywords::Float:
            init = "0.0";
            break;
        default:
            throw logic_error("Type can't be initialized.");
    }

    if (arraySize != -1) {
        code << arraySize << " DUP (" << init << ")";
    } else {
        code << init;
    }

}

void AsmCodeGenerator::evaluateExpr(shared_ptr<Node> expr, IndentOstream &code) {
    if (isArrayCall(expr->getChildAt(0))) {
        evaluateExpr(getVarDimensionExpr(expr->getChildAt(0)), code);
        code << "MOV ESI, EAX" << endl;
        code << "MOV EAX, " << getIdentifieString(expr->getChildAt(0)->getChildAt(0)) << " [ESI]" << endl;
    } else if (((InteriorNode *) expr->getChildAt(0).get())->getSort() == Sort::UnsignedInteger) {
        code << "MOV EAX, " << evalUnsignedInteger(expr) << endl;
    } else if (((InteriorNode *) expr->getChildAt(0).get())->getSort() == Sort::Variable) {
        code << "MOV EAX, " << getIdentifieString(expr) << endl;
    }
}

void AsmCodeGenerator::generateAssignment(shared_ptr<Node> assignment, IndentOstream &code) {
    evaluateExpr(assignment->getChildAt(2), code);
    if (isArrayCall(assignment->getChildAt(0))) {
        code << "PUSH EBX" << endl;
        code << "MOV EBX, EAX" << endl;
        evaluateExpr(getVarDimensionExpr(assignment->getChildAt(0)), code);
        code << "MOV ESI, EAX" << endl;
        code << "MOV " << getIdentifieString(assignment->getChildAt(0)->getChildAt(0)) << " [ESI]" << ", EBX" << endl;
        code << "POP EBX" << endl;
    } else {
        code << "MOV " << getIdentifieString(assignment->getChildAt(0)->getChildAt(0)) << "" << ", EAX" << endl;
    }
}
