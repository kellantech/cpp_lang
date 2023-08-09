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
void notImpl(string i){
  error(i+" not implemented");
}
int tt(string s){
  
  map<string,int> tt_map {
    {"ADD",1},{"SUB",2},{"MUL",3},
    {"DIV",4},{"INT",5},{"LP",6},
    {"RP",7 },{"KWD",8},{"ID",9},
    {"EQL",10},{"EQ",11},{"LCB",12},
    {"RCB",13},{"NE",14},{"GT",15},
    {"LT",16},{"AND",17},{"OR",18},
    {"CMA",19},{"DOT",20},{"STR",21},
  
    {"NONE",100},{"NUL",101}
  };
  
  return tt_map[s];
}

string rep(string x,int n){
  string r = "";
  for(;n>0;n--){
    r += x;
  }
  return r;
}

string readFile(string n) {
    ifstream inFile;
    inFile.open(n);
    stringstream strStream;
    strStream << inFile.rdbuf(); 
    inFile.close();
    return strStream.str();
}
#endif
