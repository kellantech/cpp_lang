#ifndef _LTYPE_IMPL_H
#define _LTYPE_IMPL_H

class numberType;
class stringType : public lType{
  public:
  stringType(string v){ sval = v; typ = "STR"; }
  string sget() override {return sval;} 
  void print() override{
    cout << '"' << sval << '"';
  }
  lType* add(lType* r) override{
    if (r->typ == "STR"){
      return new stringType(sval + r->sget());
    }
    else {
      notImpl("add");
    }
  }
  lType* mul(lType* r) override{
    if (r->typ == "INT"){
      return new stringType(rep(sval,r->iget()));
    }
    else {
      notImpl("mul");
    }
  }
  bool truthy() override{
    return sval.length() > 0;
  }
  int pow(int n){
    int r = 1;
    for ( int c=0; c<n; c++ ){
      r *= n;
    }
    return r;
  }
  lType* getProp(string) override;
};

class numberType: public lType{
  public:
  float ival;
  numberType(float v){ival = v; typ = "INT";}
  numberType() {error("empty constructor not allowed");}
  float iget() override{
    return ival;
  }  
  void print() override{
    cout << ival << "i";
  }
  lType* add(lType* r) override{
    if (r->typ == "INT"){
      return new numberType(ival + r->iget());
    }
    else {
      notImpl("add");
    }
  }
  lType* sub(lType* r) override{
    if (r->typ == "INT"){
      return new numberType(ival - r->iget());
    }
    else {
      notImpl("sub");
    }
  }
  lType* mul(lType* r) override{
    if (r->typ == "INT"){
      return new numberType(ival * r->iget());
    }
    else if (r->typ == "STR"){
      return new stringType(rep(r->sget(),ival));
    }
    else {
      notImpl("mul");
    }
  }
  lType* div(lType* r) override{
    if (r->typ == "INT"){
      return new numberType(ival / r->iget());
    }
    else {
      notImpl("div");
    }
  }
  lType* eql(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival == r->iget()));
    }
    else {
      notImpl("eql");
    }
  }
  lType* neq(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival != r->iget()));
    }
    else {
      notImpl("neq  ");
    }
  }
  lType* gt(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival > r->iget()));
    }
    else {
      notImpl("gt");
    }
  }
  lType* lt(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival < r->iget()));
    }
    else {
      notImpl("lt");
    }
  }
  bool truthy() override{
    return (ival != 0);
  }
  lType* getProp(string) override;
  
};

class None : public lType{
  void print() override{
    cout<<"null";  
  }
  bool truthy() override{
    return new numberType(0);
  }
};

lType* stringType::getProp(string p){
    if ( p == "len"){
      return new numberType(sval.length());
    }
    else {
      error("no property '"+p+"'");
    }
  }

class fnType : public lType{
  public:
  astNode* blk;
  vector<string> argn;
  string nm;
  fnType(astNode* bk,vector<string> ar,string n){
    blk = bk;
    argn = ar;
    nm = n;
  }
  void print() override{
    cout << "FnType " << nm << "( ";
    for (string n: argn){
      cout << n << " ";
    }
    cout << ") ";
    blk->print();
  }
  lType* call(vector<lType*> v,symbolTable st) override{
    if (v.size() > argn.size()){
      error("too many arguments");
    }
    else if (v.size() < argn.size()){
      error("too few arguments");
    }
    map<string,lType*>mp;
    symbolTable nst(mp);
    nst.setParent(&st);
    for(int i = 0; i < v.size(); i++){
      string n = argn[i];
      lType* nd = move(v[i]);
      nst.set(n,move(nd));
    }
    
    lType* rs = blk->exec(nst);
    return rs;
  }
};

 

class builtInFn : public lType{
  public:
  function<lType*(vector<lType*>)> fn;
  int argn;
  void print() override{
    cout << "<builtin>";
  }
  builtInFn(int a,
      function<lType*(vector<lType*>)> f){
      argn = a;
      fn = f;
  }
  lType* call(vector<lType*> v,symbolTable st){
    if (v.size() > argn){
      error("too many arguments");
    }
    else if (v.size() < argn){
      error("too few arguments");
    }
    return fn(v);
  }
};

lType* numberType::getProp(string p){
   if ( p == "pow"){
      return new builtInFn(1,[this](vector<lType* >x)->lType*{
        int n = ival;
        int r = 1;
        for ( int c=0; c<x[0]->iget(); c++ ){
          r *= n; 
        }
        return new numberType(r);});
    }
    else {
      error("no property '"+p+"'");
    }   
}

class listType : public lType{
  public:
  vector<lType*> lval;
  listType(vector<lType*> l){
    lval = l;
    typ = "LIST";
  }
  vector<lType*> lget() override{
    return lval;
  }
  void print() override{
    cout << "[";
    for(lType* l:lval){
      l->print();
      cout << ", ";
    }
    cout << "]";
  }
  lType* add(lType* rh) override{
   if (rh->typ == "LIST"){
     vector<lType*>r;
     for (lType* e:lval){
       r.push_back(move(e));
     }
     for (lType* e:rh->lget()){
       r.push_back(move(e));
     }
     return new listType(r);
   } 
   else{
     error("add not implemented");
   }
  }
  lType* getProp(string nm){
    if (nm == "len"){
      return new numberType(lval.size());
    }
    else {
      error("no propterty "+nm);
    }
  }
};
        
#endif
