//
// Created by Kevin on 08/10/2016.
//

#include "StringOperations.h"

#include <sstream>

using namespace Util;
using namespace std;

vector<string>& StringOperations::split(const string& s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    if (s.back() == delim) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> StringOperations::split(const string& s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}
