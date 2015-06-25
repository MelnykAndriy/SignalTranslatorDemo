//
// Created by mandriy on 5/26/15.
//

#ifndef TRANSLATORDESIGN_ASMCODEGENERATOR_H
#define TRANSLATORDESIGN_ASMCODEGENERATOR_H

#include "SyntaxTree.h"
#include <unordered_set>


class IndentOstream : public ostringstream {
public:
    friend class indentEndl;

    IndentOstream(string indent, int *indentCount) : indentCount(indentCount), indent(indent), indentIsNeeded(false) { }

    template<class Entity> friend
    IndentOstream &operator<<(IndentOstream& os,Entity ent) {
        ostringstream inner;
        if (os.indentIsNeeded) {
            os.indentIsNeeded = false;
            os.makeIndent();
        }
        inner << ent;
        if (inner.str().size())
            os.write(inner.str().c_str(), inner.str().size());
        return os;
    }


    friend ostream &endl(ostream &os);


private:
    void makeIndent();

    string indent;
    bool indentIsNeeded;
    int *indentCount;
};

class AsmCodeGenerator {
public:
    AsmCodeGenerator() : indent("    "), indentDepth(0), randomStringSize(5) { }

    string generateAsm(SyntaxTree &tree, unordered_map<unsigned int, std::shared_ptr<Token>> identifiers,
                       unordered_map<unsigned int, std::shared_ptr<Token>> constants);

    string getIndent() const {
        return indent;
    }

    void setIndent(string &indent) {
        AsmCodeGenerator::indent = indent;
    }

    string generateUniqueName();
protected:

    void generateDataSegment(SyntaxTree &tree, IndentOstream &os);
    void generateVariableType(shared_ptr<Node> declaration, IndentOstream &os);
    void evaluateExpr(shared_ptr<Node> expr, IndentOstream &os);
    void generateAssignment(shared_ptr<Node> assignment, IndentOstream &os);

private:
    unordered_set<string> names;
    string indent;
    int indentDepth;
    int randomStringSize;

};


#endif //TRANSLATORDESIGN_ASMCODEGENERATOR_H
