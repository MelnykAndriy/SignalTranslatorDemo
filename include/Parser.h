//
// Created by mandriy on 5/16/15.
//

#ifndef TRANSLATORDESIGN_PARSER_H
#define TRANSLATORDESIGN_PARSER_H

#include "Lexer.h"
#include "errors.h"
#include "grammar.h"

#include <vector>
#include <map>
#include <string>


using namespace std;

template<class Builder>
class Parser {
public:

    Parser() {
        sortFuncs[Sort::SignalProgram] = [&](Builder &builder) -> void { this->signalProgram(builder); };
        sortFuncs[Sort::Program] = [&](Builder &builder) -> void { this->program(builder); };
        sortFuncs[Sort::Block] = [&](Builder &builder) -> void { this->block(builder); };
        sortFuncs[Sort::VariableDeclarations] = [&](Builder &builder) -> void { this->variableDeclarations(builder); };
        sortFuncs[Sort::DeclarationsList] = [&](Builder &builder) -> void { this->declarationList(builder); };
        sortFuncs[Sort::Declaration] = [&](Builder &builder) -> void { this->declaration(builder); };
        sortFuncs[Sort::AttributesList] = [&](Builder &builder) -> void { this->attributesList(builder); };
        sortFuncs[Sort::Attribute] = [&](Builder &builder) -> void { this->attribute(builder); };
        sortFuncs[Sort::Range] = [&](Builder &builder) -> void { this->range(builder); };
        sortFuncs[Sort::StatementsList] = [&](Builder &builder) -> void { this->statementsList(builder); };
        sortFuncs[Sort::Statement] = [&](Builder &builder) -> void { this->statement(builder); };
        sortFuncs[Sort::Expression] = [&](Builder &builder) -> void { this->expr(builder); };
        sortFuncs[Sort::Variable] = [&](Builder &builder) -> void { this->variable(builder); };
        sortFuncs[Sort::Dimension] = [&](Builder &builder) -> void { this->dimension(builder); };
        sortFuncs[Sort::VariableIdentifier] = [&](Builder &builder) -> void { this->variableId(builder); };
        sortFuncs[Sort::ProcedureIdentifier] = [&](Builder &builder) -> void { this->procedureId(builder); };
        sortFuncs[Sort::Identifier] = [&](Builder &builder) -> void { this->identifier(builder); };
        sortFuncs[Sort::UnsignedInteger] = [&](Builder &builder) -> void { this->unsignedInt(builder); };
    }

    virtual ~Parser() { }

    Builder parse(vector<shared_ptr<Token>> tokens, Sort sort) {
        Builder builder;
        tokenIterator = begin(tokens);
        tokenEnd = end(tokens);

        try {
            sortFuncs[sort](builder);
        } catch(std::logic_error e) {
            tokenIterator -= 1;
            errorReport("Program is not finished");
        }

        if (tokenIterator != tokenEnd) {
            errorReport("Extra tokens found");
        }

        return builder;
    }


protected:

    void signalProgram(Builder &builder);

    void program(Builder &builder);

    void block(Builder &builder);

    void variableDeclarations(Builder &builder);

    void declarationList(Builder &builder);

    void declaration(Builder &builder);

    void attributesList(Builder &builder);

    void attribute(Builder &builder);

    void range(Builder &builder);

    void statementsList(Builder &builder);

    void statement(Builder &builder);

    void expr(Builder &builder);

    void variable(Builder &builder);

    void dimension(Builder &builder);

    void variableId(Builder &builder);

    void procedureId(Builder &builder);

    void identifier(Builder &builder);

    void unsignedInt(Builder &builder);

    shared_ptr<Token> currentToken() {
        if (tokenIterator != tokenEnd) {
            return *tokenIterator;
        } else {
            throw logic_error("Tokens stack is exhausted.");
        }
    }

    void nextToken() {
        if (tokenIterator == tokenEnd) {
            throw logic_error("Tokens stack is exhausted.");
        } else {
            tokenIterator++;
        }
    }

    void errorReport(string msg) {
        throw CompilerErrorsFound(
                vector<shared_ptr<TranslatorError>>(1, shared_ptr<TranslatorError>(
                        new SyntaxError(msg, currentToken()->pos))));
    }

    void strictLeafTokenCheck(function<bool(shared_ptr<Token>)> checkFunc, Builder &builder, string msg = "") {
        if (checkFunc(currentToken())) {
            builder.buildLeafNode(currentToken());
            nextToken();
        } else {
            errorReport(msg);
        }
    }

    void strictLeafTokenCodeCheck(unsigned int code, Builder &builder, string msg = "") {
        strictLeafTokenCheck([&](shared_ptr<Token> token) -> bool { return currentToken()->code == code; }, builder,
                             msg);
    }

    bool softLeafTokenCodeCheck(unsigned int code, Builder &builder) {
        return softTokenCheckCall([&](shared_ptr<Token> token) -> bool { return currentToken()->code == code; },
                                  builder,
                                  [&](Builder &builder) -> void {
                                      builder.buildLeafNode(currentToken());
                                      nextToken();
                                  });
    }

    bool softTokenCheckCall(function<bool(shared_ptr<Token>)> checkFunc, Builder &builder,
                            function<void(Builder &builder)> success) {
        if (checkFunc(currentToken())) {
            success(builder);
            return true;
        } else {
            return false;
        }
    }

    vector<shared_ptr<Token> >::iterator tokenIterator;
    vector<shared_ptr<Token> >::iterator tokenEnd;
    map<Sort, function<void(Builder &)> > sortFuncs;

};

template<class Builder>
void Parser<Builder>::signalProgram(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::SignalProgram);
    sortFuncs[Sort::Program](builder);
}

template<class Builder>
void Parser<Builder>::program(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Program);
    strictLeafTokenCodeCheck(Keyword::Keywords::Program, builder, "PROGRAM keyword is expected");
    sortFuncs[Sort::ProcedureIdentifier](builder);
    strictLeafTokenCodeCheck(Delimiter::Delimiters::Semicolon, builder, "';' is expected");
    sortFuncs[Sort::Block](builder);
    strictLeafTokenCodeCheck(Delimiter::Delimiters::Dot, builder, "'.' is expected");
}

template<class Builder>
void Parser<Builder>::block(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Block);
    sortFuncs[Sort::VariableDeclarations](builder);
    strictLeafTokenCodeCheck(Keyword::Keywords::Begin, builder, "BEGIN keyword is expected");
    sortFuncs[Sort::StatementsList](builder);
    strictLeafTokenCodeCheck(Keyword::Keywords::End, builder, "END keyword is expected");
}

template<class Builder>
void Parser<Builder>::variableDeclarations(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::VariableDeclarations);
    if (softLeafTokenCodeCheck(Keyword::Keywords::Var, builder)) {
        sortFuncs[Sort::DeclarationsList](builder);
    } else {
        builder.buildEmptyNode();
    }
}

template<class Builder>
void Parser<Builder>::declarationList(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::DeclarationsList);
    if (isIdentifierCode(currentToken()->code)) {
        sortFuncs[Sort::Declaration](builder);
        sortFuncs[Sort::DeclarationsList](builder);
    } else {
        builder.buildEmptyNode();
    }
}

template<class Builder>
void Parser<Builder>::statementsList(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::StatementsList);
    if (isIdentifierCode(currentToken()->code) || currentToken()->code == Keyword::Keywords::Loop) {
        sortFuncs[Sort::Statement](builder);
        sortFuncs[Sort::StatementsList](builder);
    } else {
        builder.buildEmptyNode();
    }
}

template<class Builder>
void Parser<Builder>::attributesList(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::AttributesList);
    if (currentToken()->code == Keyword::Keywords::Integer ||
        currentToken()->code == Keyword::Keywords::Float ||
        currentToken()->code == Delimiter::Delimiters::OpenBracket) {
        sortFuncs[Sort::Attribute](builder);
        sortFuncs[Sort::AttributesList](builder);
    } else {
        builder.buildEmptyNode();
    }
}

template<class Builder>
void Parser<Builder>::declaration(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Declaration);
    sortFuncs[Sort::VariableIdentifier](builder);
    strictLeafTokenCodeCheck(Delimiter::Delimiters::Colon, builder, "':' is expected");
    sortFuncs[Sort::Attribute](builder);
    sortFuncs[Sort::AttributesList](builder);
    strictLeafTokenCodeCheck(Delimiter::Delimiters::Semicolon, builder, "';' is expected");
}

template<class Builder>
void Parser<Builder>::attribute(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Attribute);
    if (!(softLeafTokenCodeCheck(Keyword::Keywords::Integer, builder) ||
          softLeafTokenCodeCheck(Keyword::Keywords::Float, builder))) {
        if (softLeafTokenCodeCheck(Delimiter::Delimiters::OpenBracket, builder)) {
            sortFuncs[Sort::Range](builder);
            strictLeafTokenCodeCheck(Delimiter::Delimiters::CloseBracket, builder, "']' is expected");
        } else {
            errorReport("Attribute is not found");
        }
    }
}

template<class Builder>
void Parser<Builder>::statement(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Statement);
    if (softLeafTokenCodeCheck(Keyword::Keywords::Loop, builder)) {
        sortFuncs[Sort::StatementsList](builder);
        strictLeafTokenCodeCheck(Keyword::Keywords::Endloop, builder);
    } else if (isIdentifierCode(currentToken()->code)) {
        sortFuncs[Sort::Variable](builder);
        strictLeafTokenCodeCheck(Delimiter::MultipleSymbolDelimiters::Assigne, builder, "Assign operator is expected");
        sortFuncs[Sort::Expression](builder);
    } else {
        errorReport("Invalid statement");
    }
    strictLeafTokenCodeCheck(Delimiter::Delimiters::Semicolon, builder, "Statement should be finished by ';'");
}

template<class Builder>
void Parser<Builder>::expr(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Expression);
    if (!softTokenCheckCall([](shared_ptr<Token> token) -> bool { return isIdentifierCode(token->code); }, builder,
                            sortFuncs[Sort::Variable])
        && !softTokenCheckCall([](shared_ptr<Token> token) -> bool { return isConstantCode(token->code); }, builder,
                               sortFuncs[Sort::UnsignedInteger])) {
        errorReport("Unsigned integer or identifier are expected");
    }
}

template<class Builder>
void Parser<Builder>::variable(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Variable);
    sortFuncs[Sort::VariableIdentifier](builder);
    sortFuncs[Sort::Dimension](builder);
}

template<class Builder>
void Parser<Builder>::range(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Range);
    sortFuncs[Sort::UnsignedInteger](builder);
    strictLeafTokenCodeCheck(Delimiter::MultipleSymbolDelimiters::Range, builder, "'..' is expected");
    sortFuncs[Sort::UnsignedInteger](builder);
}

template<class Builder>
void Parser<Builder>::dimension(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Dimension);
    if (softLeafTokenCodeCheck(Delimiter::Delimiters::OpenBracket, builder)) {
        sortFuncs[Sort::Expression](builder);
        strictLeafTokenCodeCheck(Delimiter::Delimiters::CloseBracket, builder, "']' is expected");
    } else {
        builder.buildEmptyNode();
    }
}

template<class Builder>
void Parser<Builder>::variableId(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::VariableIdentifier);
    sortFuncs[Sort::Identifier](builder);
}


template<class Builder>
void Parser<Builder>::procedureId(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::ProcedureIdentifier);
    sortFuncs[Sort::Identifier](builder);
}

template<class Builder>
void Parser<Builder>::identifier(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::Identifier);
    strictLeafTokenCheck([](shared_ptr<Token> token) -> bool { return isIdentifierCode(token->code); }, builder,
                         "Identifier is expected");
}

template<class Builder>
void Parser<Builder>::unsignedInt(Builder &builder) {
    auto layer = typename Builder::InteriorNodeLayer(&builder, Sort::UnsignedInteger);
    strictLeafTokenCheck([](shared_ptr<Token> token) -> bool { return isConstantCode(token->code); }, builder,
                         "Unsigned integer is expected");
}

#endif //TRANSLATORDESIGN_PARSER_H

