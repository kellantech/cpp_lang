#ifndef _CODEGEN_H
#define _CODEGEN_H
#define _CreateUIToFP builder->CreateUIToFP(LH,Type::getDoubleTy(*ctx),"convtmp");


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
    return _CreateUIToFP ;
  }
  else if (op == tt("GT")){
    LH = builder->CreateFCmpUGT(LH,RH,"cmptmp");
    return _CreateUIToFP ;
  }
  else if (op == tt("EQ")){
    LH = builder->CreateFCmpUEQ(LH,RH,"cmptmp");
    return _CreateUIToFP ;
  }
  
    
  else {
    error("BAD BINOP");
    return nullptr;
  }
}

Value* stringNode::codegen(){
  AllocaInst* aloc = builder->CreateAlloca(ArrayType::get(Type::getInt8Ty(*ctx),val.length()+1),nullptr,"__str_tmp");
  Value* cs = builder->CreateBitCast(aloc,Type::getInt8PtrTy(*ctx));
  mod->print(errs(),nullptr);
  cout << "$" << endl;
  int ind=0;
  for (char v:(val + "\0")){
    Value* i = ConstantInt::get(Type::getInt8Ty(*ctx),ind);
    Value* r = ConstantInt::get(Type::getInt8Ty(*ctx),v);
    Value* inds[1] = {i};
    Value* gep = builder->CreateGEP(Type::getInt8Ty(*ctx),cs,ArrayRef<Value*>(inds,1),"subs");
    builder->CreateStore(r,gep);
    ind++;
  } 
  return cs;
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
      int ind = 0;
      for( astNode* nd : args ){
        Value* ac = move(nd->codegen());
        Type* tp = ac->getType();
        if (tp == Type::getDoubleTy(*ctx)){
          if( fntyps[nm][ind] != "double" ){
            error("type mismatch");
          }
        }
        if (tp == Type::getInt8PtrTy(*ctx)){
          if( fntyps[nm][ind] != "str" ){
            error("type mismatch");
          }
        }
        argv.push_back(move(ac));
        if (!argv.back()){
          return nullptr;
        }
        ind++;
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
  vector<string> sts;
  for (string arg: args){
    if (arg == "str"){
      calltyp.push_back(Type::getInt8PtrTy(*ctx));
      sts.push_back("str");
    }
    else if (arg == "double"){
      calltyp.push_back(Type::getDoubleTy(*ctx));
      sts.push_back("double");
    }
    else if (arg == "i8"){
      calltyp.push_back(Type::getInt8Ty(*ctx));
      sts.push_back("i8");
    }
    else {
      error("unknown type " + arg);
      return nullptr;
    }
  }
  fntyps[nm] = sts;
  
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
  fnProtoNode* proto = new fnProtoNode(nm,vector<string>(args.size(),"double"));
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
  int ind = 0;
  for (auto& Arg: fn->args()){
    AllocaInst* aloc = addAllocaToEntry(fn,args[ind]);
    builder->CreateStore(&Arg,aloc);
    nmvals[args[ind++]] = aloc;
  }
  Value* ret = blk->codegen();
  if(ret){
    builder->CreateRet(ret);
    verifyFunction(*fn);
    if (OPT){ pm->run(*fn); }
    return fn;
  }
  fn->eraseFromParent();
  return nullptr;
}

Value* propGetNode::codegen(){
  Value* LH = nd->codegen();
  string t;
  if (LH->getType() == Type::getInt8PtrTy(*ctx)){
    t = "STR";
  }
  else if (LH->getType() == Type::getDoubleTy(*ctx)){
    t = "DBL";
  }
  else {
    error("unknown type");
    return nullptr;
  }
  astNode* f = new fnCallNode(nullptr,vector<astNode*>{new stringNode(t)},"PROPGET"+t);

  
  return f->codegen();
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
  Value* lhs = lh->codegen();
  if (op == tt("SUB")){
     return builder->CreateFMul(lhs,ConstantFP::get(*ctx,APFloat(-1.0)),"multmp");
  }
  return lhs;
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
    nmtyps[nm] = typ;
    builder->CreateStore(v,aloc);
    nmvals[nm] = aloc;
    return ConstantFP::get(*ctx,APFloat(0.0));
  }

  builder->CreateStore(v,var);
  return v;
}

Value* subsNode::codegen(){
  Value* indIR = ind->codegen();
  
  Value* cast = builder->CreateFPToSI(indIR,Type::getInt8Ty(*ctx),"cast");
  
  Value* ptr = lh->codegen();
  Value* inds[1] = {cast};
  Value* gep = builder->CreateGEP(Type::getInt8Ty(*ctx),ptr,ArrayRef<Value*>(inds,1),"subs");

  return builder->CreateLoad(Type::getInt8Ty(*ctx),gep,"ptr");
}

Value* subsSetNode::codegen(){
  
  Value* indIR = ind->codegen();
  Value* indcast = builder->CreateFPToSI(indIR,Type::getInt8Ty(*ctx),"cast");
  
  Value* ptr = lh->codegen();
  Value* inds[1] = {indcast};
  Value* gep = builder->CreateGEP(Type::getInt8Ty(*ctx),ptr,ArrayRef<Value*>(inds,1),"subs");
  
  Value* rh_ir = rh->codegen();
  Value* rh_cast = builder->CreateFPToSI(rh_ir,Type::getInt8Ty(*ctx),"cast");

  builder->CreateStore(rh_cast,gep);
  return  ConstantFP::get(*ctx,APFloat(0.0));

}



#endif
