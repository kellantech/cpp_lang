#ifndef _CODEGEN_H
#define _CODEGEN_H

AllocaInst* addAllocaToEntry(Function* fn,string name){
  IRBuilder<> tmp_builder(&fn->getEntryBlock(),
            fn->getEntryBlock().begin());
  return tmp_builder.CreateAlloca(
    Type::getDoubleTy(*ctx),nullptr,name.c_str());
}

Value* intNode::codegen(){
  return ConstantFP::get(*ctx,APFloat((double)val));
}

Value* varGetNode::codegen(){
  AllocaInst* a = nmvals[nm];
  if(!a){
    error("unknown variable "+nm);
    return nullptr;
  }
  else{
    return builder->CreateLoad(a->getAllocatedType(),a,nm.c_str());
  }
}

Value* binopNode::codegen(){
  Value* RH = r->codegen();
  Value* LH = l->codegen();
  if (!LH || !RH){
    return nullptr;
  }
  if (op == tt("ADD")){
    return builder->CreateFAdd(LH,RH,"addtmp");
  }
  else if (op == tt("SUB")){
    return builder->CreateFSub(LH,RH,"subtmp");
  }
  else if (op == tt("MUL")){
    return builder->CreateFMul(LH,RH,"multmp");
  }
  else if (op == tt("LT")){
    LH = builder->CreateFCmpULT(LH,RH,"cmptmp");
    return builder->CreateUIToFP(
      LH,Type::getDoubleTy(*ctx),"convtmp");
  }
  else {
    error("BAD BINOP");
    return nullptr;
  }
}

Value* stringNode::codegen(){
  return builder->CreateGlobalStringPtr(StringRef(val));
}
Value* listNode::codegen(){
  notImpl("codegen");
  return nullptr;
}
Value* retNode::codegen(){
  notImpl("codegen");
  return nullptr;
}
Value* blockNode::codegen(){
  if(ops.size() == 0){ 
    error("empty block");
    return nullptr;
  }
  Value* r;
  for(auto op:ops){
    r = op->codegen();
  }
  return r;
}

Value* fnCallNode::codegen(){
  Function* fn = mod->getFunction(nm);
  if (fn){
    if(fn->arg_size() == args.size()){
      vector<Value*> argv;
      for( astNode* nd : args ){
        argv.push_back(move(nd->codegen()));
        if (!argv.back()){
          return nullptr;
        }
      }
      return builder->CreateCall(fn,argv,"fncall");
    }
    else {
      error("bad num of args");
      return nullptr;
    }
  }
  else {
    error("fn not found");
    return nullptr;
  }
  notImpl("codegen not complete");
  return nullptr;
}

Function* fnProtoNode::codegen(){
  vector<Type*> calltyp;
  for (string arg: args){
    if (arg == "str"){
      calltyp.push_back(Type::getInt8PtrTy(*ctx));
    }
    else if (arg == "double"){
      calltyp.push_back(Type::getDoubleTy(*ctx));
    }
    else {
      error("unknown type " + arg);
    }
  }
  
  FunctionType* ft = 
      FunctionType::get(
                  Type::getDoubleTy(*ctx),calltyp,false);
  Function* f = 
      Function::Create(
        ft,Function::ExternalLinkage,nm,mod.get());
  int i = 0;
  for(auto& arg: f->args()){
    arg.setName(args[i]);
    i++;
  }
  return f;
}

Value* fnDefNode::codegen(){
  fnProtoNode* proto = new fnProtoNode(nm,args);
  Function* fn = mod->getFunction(proto->nm);
  if (!fn){ fn = proto->codegen(); }
  if (!fn){ return nullptr; }
  if (!fn->empty()){
    error("cant redefine fn");
    return nullptr;
  }
  BasicBlock* bb = BasicBlock::Create(*ctx,"entry",fn);
  builder->SetInsertPoint(bb);
  nmvals.clear();
  for (auto& Arg: fn->args()){
    AllocaInst* aloc = addAllocaToEntry(fn,string(Arg.getName()));
    builder->CreateStore(&Arg,aloc);
    nmvals[string(Arg.getName())] = aloc;
  }
  Value* ret = blk->codegen();
  if(ret){
    builder->CreateRet(ret);
    verifyFunction(*fn);
    return fn;
  }
  fn->eraseFromParent();
  return nullptr;
}

Value* propGetNode::codegen(){
  notImpl("codegen");
  return nullptr;
}

Value* ifNode::codegen(){
  Value* cond = cnd->codegen();
  if (!cond){
    return nullptr;
  }
  cond = builder->CreateFCmpONE(cond,
    ConstantFP::get(*ctx,APFloat(0.0)),"ifcond");

  Function* fn = builder->GetInsertBlock()->getParent();
  
  BasicBlock *BBthen = BasicBlock::Create(*ctx,"then",fn);
  BasicBlock *BBelse = BasicBlock::Create(*ctx,"else");
  BasicBlock *BBmerge = BasicBlock::Create(*ctx,"merge");
  builder->CreateCondBr(cond,BBthen,BBelse);

  builder->SetInsertPoint(BBthen);
  Value* thenIR = blk->codegen();
  if (!thenIR){ return nullptr; }
  builder->CreateBr(BBmerge);

  BBthen = builder->GetInsertBlock();

  fn->getBasicBlockList().push_back(BBelse);
  builder->SetInsertPoint(BBelse);
  Value* elseIR = ebk->codegen();
  if (!elseIR){ return nullptr; }
  
  builder->CreateBr(BBmerge);
  BBelse = builder->GetInsertBlock();

  
  fn->getBasicBlockList().push_back(BBmerge);
  builder->SetInsertPoint(BBmerge);
  PHINode* phi = builder->CreatePHI(
            Type::getDoubleTy(*ctx),2,"mergetmp");
 
  phi->addIncoming(thenIR,BBthen);
  phi->addIncoming(elseIR,BBelse);
  
  return phi;
}



Value* whileNode::codegen(){
  Function* fn = builder->GetInsertBlock()->getParent();
  BasicBlock* BBcnd = BasicBlock::Create(*ctx,"cnd",fn);
  BasicBlock* BBloop = BasicBlock::Create(*ctx,"loop");
  BasicBlock* BBmerge = BasicBlock::Create(*ctx,"merge");

  builder->CreateBr(BBcnd);
  builder->SetInsertPoint(BBcnd);
  Value* cndIR = cnd->codegen();
  cndIR = builder->CreateFCmpONE(
      cndIR,ConstantFP::get(*ctx,APFloat(0.0)),"whiletmp");
  builder->CreateCondBr(cndIR,BBloop,BBmerge);

  fn->getBasicBlockList().push_back(BBloop);
  builder->SetInsertPoint(BBloop);
  Value* blkIR = bk->codegen();
  builder->CreateBr(BBcnd);

  fn->getBasicBlockList().push_back(BBmerge);
  builder->SetInsertPoint(BBmerge);
  return ConstantFP::get(*ctx,APFloat(0.0));
}

Value* forNode::codegen(){
  Value* initIR = init->codegen();
    
  Function* fn = builder->GetInsertBlock()->getParent();
  BasicBlock* BBcnd = BasicBlock::Create(*ctx,"cnd",fn);
  BasicBlock* BBloop = BasicBlock::Create(*ctx,"loop");
  BasicBlock* BBmerge = BasicBlock::Create(*ctx,"merge");

  builder->CreateBr(BBcnd);

  
  builder->SetInsertPoint(BBcnd);
  Value* cndIR = cnd->codegen();
  cndIR = builder->CreateFCmpONE(cndIR,
    ConstantFP::get(*ctx,APFloat(0.0)),"cmp");
  builder->CreateCondBr(cndIR,BBloop,BBmerge);

  fn->getBasicBlockList().push_back(BBloop);
  builder->SetInsertPoint(BBloop);
  blk->codegen();
  inc->codegen();
  builder->CreateBr(BBcnd);
  
  fn->getBasicBlockList().push_back(BBmerge);
  builder->SetInsertPoint(BBmerge);

  
  return ConstantFP::get(*ctx,APFloat(0.0));
}

Value* unOpNode::codegen(){
  notImpl("codegen");
  return nullptr;
}


Value* varSetNode::codegen(){
  Value* v = val->codegen();
  if (!v){ return nullptr; }

  Value* var = nmvals[nm];
  if (!var){
    Function *fn = builder->GetInsertBlock()->getParent();

    Value* init = val->codegen();
    if (!init){ return nullptr; }
    
    AllocaInst* aloc = addAllocaToEntry(fn,nm);
    builder->CreateStore(init,aloc);
    nmvals[nm] = aloc;
    return ConstantFP::get(*ctx,APFloat(0.0));
  }

  builder->CreateStore(v,var);
  return v;
}


Value* TypVarSetNode::codegen(){
  Value* v = val->codegen();
  if (!v){ return nullptr; }

  Value* var = nmvals[nm];
  if (!var){
    Function *fn = builder->GetInsertBlock()->getParent();
    Value* init = val->codegen();
    if (!init){ return nullptr; }
    IRBuilder<> tmp_builder(&fn->getEntryBlock(),
            fn->getEntryBlock().begin());
    AllocaInst* aloc;
    if (typ == "double"){
      aloc = tmp_builder.CreateAlloca(
          Type::getDoubleTy(*ctx),nullptr,nm.c_str());
    }
    else if ( typ == "str"){
      aloc = tmp_builder.CreateAlloca(
          Type::getInt8PtrTy(*ctx),nullptr,nm.c_str());
    }
    else {
      error("unknown type " + typ);
    }
    builder->CreateStore(init,aloc);
    nmvals[nm] = aloc;
    return ConstantFP::get(*ctx,APFloat(0.0));
  }

  builder->CreateStore(v,var);
  return v;
}



#endif
