#ifndef _MODULE_H
#define _MODULE_H

static unique_ptr<LLVMContext> ctx;
static unique_ptr<IRBuilder<>> builder;
static unique_ptr<Module> mod;
static map<string,AllocaInst*> nmvals;
static map<string,string> nmtyps;
static map<string,vector<string>> fntyps;


#include "codegen.hpp"


void initModule(){
  ctx = make_unique<LLVMContext>();
  mod = make_unique<Module>("module",*ctx);
  builder = make_unique<IRBuilder<>>(*ctx);
}



void genObj(string f){
  string err;
  auto targetStr = sys::getDefaultTargetTriple();
  auto target = TargetRegistry::lookupTarget(targetStr,err);

  if (!target){
    errs() << err;
    exit(1);
  }
  string cpu = "generic";
  string features = "";
  TargetOptions opt;
  auto relocModel = Optional<Reloc::Model>();
  auto machine = target->createTargetMachine(targetStr,cpu,features,opt,relocModel);

  mod->setDataLayout(machine->createDataLayout());
  mod->setTargetTriple(targetStr);

  string fn = f;
  error_code ec;
  raw_fd_ostream fh(fn,ec,sys::fs::OF_None);

  if(ec){
    errs() << "could not open file: " << ec.message();
    exit(1);
  }
  legacy::PassManager pass;
  auto ft = CGFT_ObjectFile;

  auto er = machine->addPassesToEmitFile(pass,fh,nullptr,ft);
  if (er){
    errs() << "cant emit object file";
    exit(1);
  }
  pass.run(*mod);
  fh.flush();
}


#endif
