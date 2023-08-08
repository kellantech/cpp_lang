#include <iostream>
#include <map>
#include <vector>
#include <functional>
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





lType* test(vector<lType*> inp){
  return new stringType(":) !!");
}

///
int main() {
  map<string,lType*> m;
  symbolTable gst(m);
  builtInFn tst(0,&test);
  gst.set("test", &tst);
string inp;
  while (1){
    inp = "";
    cout<<"lexer parser inter0> ";
    getline(cin,inp);
    cout<<inp<<"!"<<endl;
    lexer ls (inp);
    vector<token>r = ls.gen_toks();
    for(auto e:r) e.print();
    
    cout<<endl;
    parser p (r);
    astNode* exp = p.logical_expr();
    exp -> print();
    cout<<endl;
    exp -> exec(gst) -> print();
    cout<<endl;
    ///gst.get("x") -> print();
    }
}
