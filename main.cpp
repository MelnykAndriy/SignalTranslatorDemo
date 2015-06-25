#include <iostream>
#include <boost/program_options.hpp>
#include <fstream>

#include "include/Lexer.h"
#include "include/Parser.h"
#include "include/grammar.h"
#include "include/SyntaxTree.h"
#include "include/GraphTree.h"
#include "include/SemanticChecker.h"
#include "include/AsmCodeGenerator.h"

using namespace std;
using namespace boost::program_options;

ostream &operator<<(ostream &os, const Position &pos) {
    os << "l : " << pos.line << ", c : " << pos.column;
    return os;
}

ostream &operator<<(ostream &os, const Token &token) {
    os << "rep : " << token.rep << ", code : " << token.code << ", pos : " << token.pos;
    return os;
}

int main(int argc, char const *argv[]) {
    string fileToCompile;
    string dotFile;

    options_description desc("Client options");
    desc.add_options()
            ("help,h", "produce help message")
            ("file,f", value<string>(&fileToCompile)->required(), "Source file to be compiled")
            ("output,o", value<string>(), "Name of output file")
            ("lexer,l", bool_switch()->default_value(false), "Wheater dump lexer result or not.")
            ("parser,p", value<string>(&dotFile), "Path where tree will be dumped in dot format.");
    variables_map args;
    try {
        parsed_options parsed = command_line_parser(argc, argv).options(desc).run();
        store(parsed, args);
        notify(args);
    } catch (exception &e) {
        cerr << desc << endl;
        exit(EXIT_FAILURE);
    }
    if (args.count("help")) {
        cerr << desc << endl;
        exit(EXIT_SUCCESS);
    }
    vector<string> sourceFileByLines;
    try {
        ifstream source_file(fileToCompile);
        if (!source_file.is_open() or !source_file.good()) {
            throw exception();
        }
        while (source_file.good()) {
            string line;
            getline(source_file, line);
            sourceFileByLines.push_back(line);
        }
    } catch (exception &e) {
        cerr << "Source file read error." << endl;
        exit(EXIT_FAILURE);
    }

    Lexer lex;
    Parser<SyntaxTreeBuilder> parser;
    SemanticChecker checker;
    AsmCodeGenerator generator;

    try {
        auto tokens = lex.lexicalAnalysis(sourceFileByLines);
        if (args["lexer"].as<bool>()) {
            cout << "Tokens: " << endl;
            for (auto token: tokens) {
                cout << *token << endl;
            }
            cout << "Identifiers table: " << endl;
            for (auto identifier: lex.getIdentifiersTable()) {
                cout << identifier.first << " : " << identifier.second->rep << endl;
            }
            cout << "Constants table: " << endl;
            for (auto constant: lex.getConstantsTable()) {
                cout << constant.first << " : " << constant.second->rep << endl;
            }

        }
        auto tree = parser.parse(tokens, Sort::SignalProgram).getTree();
        if (dotFile != "") {
            Parser<GraphVizTreeBuilder> graphVizParser;
            ofstream dot(dotFile);
            dot << graphVizParser.parse(tokens, Sort::SignalProgram).getTree();
        }
        checker.check(tree);

        string outFileName;
        if (args.count("output")) {
            outFileName = args["output"].as<string>();
        } else {
            outFileName = fileToCompile + ".asm";
        }
        ofstream outputFile(outFileName);
        outputFile << generator.generateAsm(tree, lex.getIdentifiersTable(), lex.getConstantsTable());
    } catch (CompilerErrorsFound &e) {
        dumpErrors(e.getErrors());
        exit(EXIT_FAILURE);
    }

    return 0;
}