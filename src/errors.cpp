//
// Created by mandriy on 5/16/15.
//

#include "../include/errors.h"
#include <iostream>

void dumpErrors(const vector<shared_ptr<TranslatorError> > &errors) {
    for (auto error_ptr: errors) {
        cerr << *error_ptr << endl;
    }
}