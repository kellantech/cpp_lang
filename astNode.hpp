
class astNode{
  public:
  virtual bool isRet(){
    return false;
  }
  virtual lType* exec( symbolTable st ){
    cout<<":("<<endl;
    error("...");
  }
  virtual void print(){
    cout << ":(p"<<endl;
  }
};
