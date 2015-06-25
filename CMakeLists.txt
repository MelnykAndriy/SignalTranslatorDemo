cmake_minimum_required(VERSION 3.1)
project(TranslatorDesign)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lboost_program_options")

set(SOURCE_FILES main.cpp include/errors.h include/utils.h include/Lexer.h include/Parser.h src/Lexer.cpp src/utils.cpp src/errors.cpp)
add_executable(TranslatorDesign ${SOURCE_FILES} include/SyntaxTree.h src/SyntaxTree.cpp include/GraphTree.h src/GraphTree.cpp include/grammar.h include/SemanticChecker.h src/SemanticChecker.cpp src/AsmCodeGenerator.cpp include/AsmCodeGenerator.h)