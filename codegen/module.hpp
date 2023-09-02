#ifndef _MODULE_H
#define _MODULE_H

static unique_ptr<LLVMContext> ctx;
static unique_ptr<IRBuilder<>> builder;
static unique_ptr<Module> mod;
static map<string,Value*> nmvals;
static unique_ptr<legacy::FunctionPassManager> FPM;
#include "codegen.hpp"

bool modinit = false;

void runCG(astNode* nd){
  if(!modinit){
    ctx = make_unique<LLVMContext>();
    mod = make_unique<Module>("module",*ctx);
    builder = make_unique<IRBuilder<>>(*ctx);
    FPM = make_unique<legacy::FunctionPassManager>(mod.get());
    
    FPM->add(createInstructionCombiningPass());
    FPM->add(createReassociatePass());
    FPM->add(createGVNPass());
    FPM->add(createCFGSimplificationPass());
    FPM->doInitialization();
    modinit = true;
  }
  auto* ir = nd->codegen();
  ir->print(errs());
}


#endif
