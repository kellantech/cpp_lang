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


int main(int argc ,char** argv) {
  bool INFO = 0;
  bool INTERP = 1;
  bool IR = 0;
  bool LINK = 0;
  string FILE;
  
  for (int i = 1; i < argc; i++){
    string arg = argv[i];
    if ( arg == "-i" || arg == "--info"){
      INFO = true;
    }
    else if (arg == "-c"){
      INTERP = 0;
    }
    else if (arg == "--ir"){
      IR = true;
    }
    else if (arg == "-l"){
      LINK = true;
    }
    else{
      FILE = arg;
    }
  }
  if(FILE == "" && LINK) { FILE = "a.out";}
  else if (FILE == "") { FILE = "out.o"; }
  if(!INTERP){
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
  }
  vector<string> b;
  map<string,lType*> m;
  symbolTable gst(m);
  if (INTERP){
    gst.set("print", new builtInFn(1,&print));
    gst.set("input", new builtInFn(1,&input));
  }
  initModule();
  
  string inp = readFile("main.?");
  
  // string inp = "";
  // inp = "";
  // cout << "> ";
  // getline(cin,inp);
  
  auto tp = imp(inp);
  inp = get<0>(tp);
  b = get<1>(tp);
  for (string mn: b){
    if (INTERP) { getMod(mn,gst); }
  };
  lexer ls (inp);
  vector<token>r = ls.gen_toks();
  if (INFO){  
    for(auto e:r){ e.print(); }
      cout<<endl;
  }
  parser p (r);
  while(1){
  astNode* exp = p.logical_expr();
  if (INFO){ exp->print(); }
  if (!INTERP) { exp->codegen(); }
  if (INTERP) { exp->exec(gst); }
  if(p.cur.type == tt("NONE")){
    break;
  }
  }
  if (!INTERP){
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    
    if (IR) { mod->print(errs(),nullptr); }
    if (!LINK) { genObj(FILE); }
    if (LINK) {
      char ff[] = "/tmp/fileXXXXXX";
      int fd = mkstemp(ff);
      genObj(ff);
      cout << "%" << endl;
      system(("clang++ libio.so "+ string(ff)+" -o " + FILE +" -Wl,-rpath,`pwd` ").c_str());
    }
  }
  
}
