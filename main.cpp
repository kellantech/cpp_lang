#define INFO 1

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream> 
#include <regex>
#include <filesystem>
#include <tuple>
#include <cmath>
//////
using namespace std;


#include "utils.hpp"

class lType;
class astNode;
class symbolTable;


#include "symbolTable.hpp"
#include "types/lType_base.hpp"
#include "ast/astNode_base.hpp"

#include "types/lType.hpp"
#include "ast/astNode.hpp"

#include "parser/token.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

#include "preprocess.hpp"
#include "getMod.hpp"


lType* print(vector<lType*> inp){
  inp[0]->print();
  cout << endl;
  return new None;
}

lType* input(vector<lType*> inp){
  print(inp);
  string in;
  cin >> in;
  return new stringType(in);
}

int main(int argc ,char** argv) {
  vector<string> b;
  map<string,lType*> m;
  symbolTable gst(m);
  gst.set("print", new builtInFn(1,&print));
  gst.set("input", new builtInFn(1,&input));
  
  string inp = readFile("main.?");
  auto tp = imp(inp);
  inp = get<0>(tp);
  b = get<1>(tp);
  for (string mn: b){
    getMod(mn,gst);
  };
  lexer ls (inp);
  vector<token>r = ls.gen_toks();
  if (INFO){  
    for(auto e:r) e.print();
    cout<<endl;
  }
  parser p (r);
  astNode* exp = p.block();
  if (INFO){ exp->print(); }
  cout<<endl;
  exp->exec(gst)->print();
  cout<<endl;
    
}///////////////
