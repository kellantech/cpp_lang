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
      ntop("+","str",r->typ);
      return nullptr;
    }
  }
  lType* mul(lType* r) override{
    if (r->typ == "INT"){
      return new stringType(rep(sval,r->iget()));
    }
    else {
      ntop("*","str",r->typ);
      return nullptr;
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
  lType* eql(lType*) override;
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
      ntop("+","int",r->typ);
      return nullptr;
    }
  }
  lType* sub(lType* r) override{
    if (r->typ == "INT"){
      return new numberType(ival - r->iget());
    }
    else {
      ntop("-","int",r->typ);
      return nullptr;
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
      ntop("*","int",r->typ);
      return nullptr;
    }
  }
  lType* div(lType* r) override{
    if (r->typ == "INT"){
      return new numberType(ival / r->iget());
    }
    else {
      ntop("/","int",r->typ);
      return nullptr;
    }
  }
  lType* eql(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival == r->iget()));
    }
    else {
      ntop("==","int",r->typ);
      return nullptr;
    }
  }
  lType* neq(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival != r->iget()));
    }
    else {
      ntop("!=","int",r->typ);
      return nullptr;
    }
  }
  lType* gt(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival > r->iget()));
    }
    else {
      ntop(">","int",r->typ);
      return nullptr;
    }
  }
  lType* lt(lType* r) override{
    if (r->typ == "INT"){
      return new numberType((int)(ival < r->iget()));
    }
    else {
      ntop("<","int",r->typ);
      return nullptr;
    }
  }
  bool truthy() override{
    return (ival != 0);
  }
  lType* getProp(string) override;
  
};

class lNone : public lType{
  void print() override{
    cout<<"null";  
  }
  bool truthy() override{
    return false;
  }
};

lType* stringType::getProp(string p){
    if ( p == "len"){
      return new numberType(sval.length());
    }
    else {
      error("no property '"+p+"'");
      return nullptr;
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
  lType* call(vector<lType*> v,symbolTable st) override {
    if (v.size() > argn){
      error("too many arguments");
    }
    else if (v.size() < argn){
      error("too few arguments");
    }
    return fn(v);
  }
};
lType* stringType::eql(lType* r){
    if (r->typ == "STR"){
      return new numberType(r->sget() == sval);
    }
    else {
      notImpl("eql");
      return nullptr;
    }
 }

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
      return nullptr;
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
     ntop("+","list",rh->typ);
     return nullptr;
   }
  }
  lType* getProp(string nm) override {
    if (nm == "len"){
      return new numberType(lval.size());
    }
    else if (nm == "get"){
      return new builtInFn(1,[this](vector<lType*> v)->lType*{
        return this->lval[v[0]->iget()];
      });
    }
    else if (nm == "set") {
      return new builtInFn(2,[this](vector<lType*> v)->lType*{
        this->lval[v[0]->iget()] = v[1];
        return new lNone;
      });
    }
    else if (nm == "del") {
      return new builtInFn(1,[this](vector<lType*> v)->lType*{
        int ind = v[0]->iget();
        vector<lType*> tmp;
        for(int i = 0; i < ind; i++){
          tmp.push_back(move(lval[i]));
        }
        for(int i = ind + 1; i < lval.size(); i++){
          tmp.push_back(move(lval[i]));
        }
        
        lval = tmp;
        return new lNone;
      });
    }
    else if (nm == "insert") {
      return new builtInFn(2,[this](vector<lType*> v)->lType*{
        int ind = v[0]->iget();
        if (ind >= lval.size()){
          error("list index out of range");
        }
        vector<lType*> tmp;
        for(int i = 0; i <= ind; i++){
          tmp.push_back(move(lval[i]));
        }
        tmp.push_back(v[1]);
        
        for(int i = ind+1; i < lval.size(); i++){
          tmp.push_back(move(lval[i]));
        }
        
        lval = tmp;
        return new lNone;
      });
    }
    else if (nm == "reverse"){
      return new builtInFn(0,
        [this](vector<lType*>v)->lType*{
        vector<lType*>tmp;
        for(lType* l: lval){
          
 tmp.insert(tmp.begin(),move(l));
        }
        lval = tmp;
        return new lNone;
      });
    }
    else if (nm == "sort"){
      return new builtInFn(0,
        [this](vector<lType*> v)->lType*{     
          vector<lType*>t;
          while (true){
            
            if (lval.size() == 0){
              break;
            }
            lType* e = lval.back();
            lval.pop_back();
            if (t.size() == 0){
              t.push_back(e);
            }
            else{
             int i = 0;
             for (lType* s : t){
               if (e->gt(s)->truthy()){
                  t.insert(t.begin()+i,e);
                  goto done;
               }  
               ++i;
             } 
            t.push_back(e);
            done:;
            } 
          }
          lval = t;
          return new lNone;
      });
    }
    else {
      error("no propterty "+nm);
      return nullptr;
    }
  }
};
        
#endif
