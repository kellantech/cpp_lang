#ifndef _AST_H
#define _AST_H
class astNode{
  public:
  virtual bool isRet(){
    return false;
  }
  virtual lType* exec( symbolTable st ){
    cout<<":("<<endl;
    error("...");
    return nullptr;
  }
  virtual void print(){
    cout << ":(p"<<endl;
  }
  virtual Value* codegen() = 0; 
  virtual void jit_exec(){
    error("jit exec not implemented");
  }
};
#endif
