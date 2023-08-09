#define INFO 1

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream> 
#include <regex>


using namespace std;

//
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




lType* print(vector<lType*> inp){
  inp[0]->print();
  cout << endl;
  return new None;
}

///
int main(int argc ,char** argv) {///////////
  map<string,lType*> m;
  symbolTable gst(m);
  builtInFn printFN(1,&print);
  gst.set("print", &printFN);
  string inp = readFile("main.?");
  cout << "!" << endl;
  inp = imp(inp);
  lexer ls (inp);
  cout << '!'<<endl;
  vector<token>r = ls.gen_toks();
  if (INFO){  
    for(auto e:r) e.print();
    cout<<endl;
  }//
  parser p (r);
  astNode* exp = p.block();
  if (INFO){ exp -> print(); }
  cout<<endl;
  exp -> exec(gst) -> print();
  cout<<endl;
    ///
}
