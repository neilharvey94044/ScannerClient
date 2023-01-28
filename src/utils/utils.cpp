// Copyright (c) Neil D. Harvey

#include <string>
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




} //namespace