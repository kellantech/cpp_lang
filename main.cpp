#define INFO 1
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"


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
#include <cstdio>
#include <memory>

using namespace std;
using namespace llvm;
#include "utils.hpp"

class lType;
class astNode;
class symbolTable;

///////
#include "symbolTable.hpp"
#include "types/lType_base.hpp"
#include "ast/astNode_base.hpp"

#include "types/lType.hpp"
#include "ast/astNode.hpp"
#include "codegen/module.hpp"


#include "parser/token.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

#include "preprocess.hpp"
#include "getMod.hpp"


lType* print(vector<lType*> inp){
  inp[0]->print();
  cout << endl;
  return new lNone;
}

lType* input(vector<lType*> inp){
  print(inp);
  string in;
  cin >> in;
  return new stringType(in);
}

void loop(){
}

int main(int argc ,char** argv) {
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  InitializeNativeTargetAsmParser();

  
  vector<string> b;
  map<string,lType*> m;
  symbolTable gst(m);
  gst.set("print", new builtInFn(1,&print));
  gst.set("input", new builtInFn(1,&input));

  initModule();
  //string inp = readFile("main.?");
  
  string inp = "";
  inp = "";
    cout << "> ";
  getline(cin,inp);
  
  while(1){
    
    
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
    astNode* exp = p.logical_expr();
    if (INFO){ exp->print(); }
    cout<<endl;
    runCG(exp);
    cout << endl;
    //exp->exec(gst)->print();
    inp = "";
    cout << "> ";
    getline(cin,inp);
    
    if(inp == "q"){
        break;
    }
  }
  InitializeAllTargetInfos();
  InitializeAllTargets();
  InitializeAllTargetMCs();
  InitializeAllAsmParsers();
  InitializeAllAsmPrinters();
  
  cout << "********************" << endl;
  mod->print(errs(),nullptr);
  cout << "********************" << endl;
  
  genObj();
}
