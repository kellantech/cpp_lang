#ifndef _CODEGEN_H
#define _CODEGEN_H

Value* intNode::codegen(){
  return ConstantFP::get(*ctx,APFloat(val));
}

Value* varGetNode::codegen(){
  Value* v = nmvals[nm];
  if(!v){
    error("unknown variable "+nm);
    return nullptr;
  }
  else{
    return v;
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
  notImpl("codegen");
  return nullptr;
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
  vector<Type*> calltyp (args.size(),Type::getDoubleTy(*ctx));
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
  for (auto& Arg: fn->args()){
    nmvals[string(Arg.getName())] = &Arg;
  }
  Value* ret = blk->codegen();
  if(ret){
    builder->CreateRet(ret);
    verifyFunction(*fn);

    FPM->run(*fn);
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
  notImpl("codegen");
  return nullptr;
}



Value* whileNode::codegen(){
  notImpl("codegen");
  return nullptr;
}

Value* forNode::codegen(){
  notImpl("codegen");
  return nullptr;
}

Value* unOpNode::codegen(){
  notImpl("codegen");
  return nullptr;
}


Value* varSetNode::codegen(){
  notImpl("codegen");
  return nullptr;
}


#endif
