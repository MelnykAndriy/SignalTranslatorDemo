//
// Created by mandriy on 5/25/15.
//

#ifndef TRANSLATORDESIGN_GRAMMAR_H
#define TRANSLATORDESIGN_GRAMMAR_H

#include <boost/assign/list_of.hpp>
#include <map>

using namespace std;

enum class Sort {
    SignalProgram,
    Program,
    Block,
    VariableDeclarations,
    DeclarationsList,
    Declaration,
    AttributesList,
    Attribute,
    Range,
    StatementsList,
    Statement,
    Expression,
    Variable,
    Dimension,
    VariableIdentifier,
    ProcedureIdentifier,
    UnsignedInteger,
    Identifier
};

const map<Sort, string> sortsLabels = boost::assign::map_list_of
        (Sort::SignalProgram, "signal-program")
        (Sort::Program, "program")
        (Sort::Block, "block")
        (Sort::VariableDeclarations, "variable-declarations")
        (Sort::DeclarationsList, "declarations-list")
        (Sort::Declaration, "declaration")
        (Sort::AttributesList, "attributes-list")
        (Sort::Attribute, "attribute")
        (Sort::Range, "range")
        (Sort::StatementsList, "statements-list")
        (Sort::Statement, "statement")
        (Sort::Expression, "expr")
        (Sort::Variable, "variable")
        (Sort::Dimension, "dimension")
        (Sort::VariableIdentifier, "variable-identifier")
        (Sort::ProcedureIdentifier, "procedure-identifier")
        (Sort::Identifier, "identifier")
        (Sort::UnsignedInteger, "unsigned-integer");

#endif //TRANSLATORDESIGN_GRAMMAR_H
