// Copyright (c) Neil D. Harvey
// SPDX-License-Identifier: GPL-2.0+

#include <fstream>
#include <string>
#include "spdlog/spdlog.h"
#include "utils.h"

using namespace std;
namespace sc {
void trim (string& field){
    field.erase(field.begin(), std::find_if(field.begin(), field.end(), [](int ch) {
      return !std::isspace(ch);
    }));
    field.erase(std::find_if(field.rbegin(), field.rend(), [](int ch) {
      return !std::isspace(ch);
    }).base(), field.end());

}

// Utility function to dump data received to a file
void dump(const string fname, const string outbuf) {
    spdlog::debug("dump() dumping message to file {}", fname);
    std::fstream fs;
    fs.open(fname, std::ios::binary | std::ios::out);
    if(!fs.is_open()){
        spdlog::error("Unable to open file: {}", fname);
        return;
    }
    fs << outbuf;
    fs.close();
    
}

// Utility function to replace control characters with spaces
void stripctrlchars(std::string& msg) {
    spdlog::debug("Stripping control characters.");
    for(char& c: msg){
        if (iscntrl(c) != 0) {c = 0x20;}

    }
}


} //namespace