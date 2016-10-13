//
// Created by Kevin on 13/10/2016.
//

#include <fstream>
#include <stdexcept>

#include "FileOperations.h"

using namespace std;
using namespace Util;

vector<uint8_t> FileOperations::loadBinaryFile(const string& filePath) {
    vector<uint8_t> data;

    ifstream file (filePath, ios::binary | ios::ate);

    if (!file) {
        throw runtime_error("Cannot open file: " + filePath);
    }

    //file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    data.resize(length);
    if (!file.read(reinterpret_cast<char *>(data.data()), length)) {
        throw runtime_error("Cannot read data from file: " + filePath);
    }

    return data;
}