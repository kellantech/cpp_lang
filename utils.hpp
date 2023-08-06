#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;


template <typename T>
bool findvec(vector<T> vec, T e) {
  for (T el : vec){
    if ( e == el){  return true; }
  }
  return false;
}


template <typename T>
bool findary(T ary [], T e, int s) {
  for (int i =0;i<s;i++){
    if ( e == ary[i]){return true;}
  }
  return false;

}

void error(string msg){
  cout << "error: " << msg << endl;
  exit(1);
}
int tt(string s){
  
  map<string,int> tt_map {
    {"ADD",1},{"SUB",2},{"MUL",3},
    {"DIV",4},{"INT",5},{"LP",6},
    {"RP",7 },{"KWD",8},{"ID",9},
    {"EQL",10},{"EQ",11},{"LCB",12},
    {"RCB",13},{"NE",14},{"GT",15},
    {"LT",16},{"AND",17},{"OR",18},
    {"CMA",19},{"DOT",20},
  
    {"NONE",100},{"NUL",101}
  };
  
  return tt_map[s];
}
#endif
