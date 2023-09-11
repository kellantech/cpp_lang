#ifndef _ASTNODE_H
#define _ASTNODE_H
class intNode : public astNode{
  public:
  float val;
  intNode(float v){val = v;}
  void print() override {
    cout << val << " ";
  }
  lType* exec(symbolTable st) override{
    return new numberType(val);
  }
  Value* codegen() override;
  
};
class stringNode : public astNode{
  public:
  string val;
  stringNode(string v){ val = v; }
  void print() override{
    cout << "'" << val << "'";
  }
  lType* exec(symbolTable st) override{
    return new stringType(val);
  }
  Value* codegen() override;
};

class binopNode : public astNode{
  public:
  astNode* l;
  astNode* r;
  int op;
binopNode(astNode* lh,astNode* rh,int o){l = move(lh);r=move(rh);op=o;}
  void print() override{
    cout << "[ ";
    l->print() ;
 
    cout<<op << " ";

    r->print();
    cout << "]";
  }
  lType* exec(symbolTable st) override{
    lType* lh = l->exec(st);
    lType* rh = r->exec(st);
    lType* r;
    if (op == tt("ADD")){
      r =  ( (lh)->add(rh) );
    }
    else if (op == tt("SUB")){
      r =  ( (lh)->sub(rh) );
    }
    else if (op == tt("MUL")){
      r =  ( (lh)->mul(rh) );
    }
    else if (op == tt("DIV")){
      r =  ( (lh)->div(rh) );
    }
    else if (op == tt("EQ")){
      r =  ( (lh)->eql(rh) );
    }
    else if (op == tt("NE")){
      r =  ( (lh)->neq(rh) );
    }
    else if (op == tt("LT")){
      r =  ( (lh)->lt(rh) );
    }
    else if (op == tt("GT")){
      r =  ( (lh)->gt(rh) );
    }
    else if ( op == tt("AND")){
      bool lt = lh->truthy();
      bool rt = rh->truthy();
      return new numberType(lt && rt);
    }
    else if ( op == tt("OR")){
      return new numberType(lh->truthy() || rh->truthy());
    }
    else {error("u");}
    
    return r;
  }
  Value* codegen() override;
};

class blockNode : public astNode{
public:
  vector<astNode*> ops;
  bool fn;
  blockNode( vector<astNode*> op,bool f=false){
    fn = f;
    for(astNode* el:op){
      ops.push_back(move(el));
    }
  }
  lType* exec(symbolTable st) override{
    for(astNode* op : ops){
      if (op->isRet()){
        if (fn){ return op->exec(st); }
        else { 
          error("can only return from functions"); 
        }
      }
      else{
        op->exec(st);
      }
    }
    return new lNone();
  }
  void print() override{
  cout<<"{";
    for(auto* i:ops){
      i->print();
    }
    cout<<"}";
  }
Value* codegen() override;

};

class unOpNode : public astNode{
  public:
  int op;
  astNode* lh;
  unOpNode(int o,astNode* l){
    op = o;
    lh = move(l);
  }
  lType* exec(symbolTable st) override{
    lType* l = lh->exec(st);
    if (op == tt("SUB")){
      l = l->mul(new numberType(-1));
    }
    return move(l);
  }
  void print() override {
    cout << "[" <<  ((op == tt("SUB"))?"-":"+") ;
    lh->print();
    cout <<  "]"; 
  }
Value* codegen() override;
};

class varSetNode : public astNode{
public:
string nm;
astNode* val;
 varSetNode(string n ,astNode* vl){
   nm = n;
   val = move(vl);
 }
  lType* exec(symbolTable st) override{
    lType* l = val->exec(st);
    st.set(nm,move(l));
    return new lNone();
  }
  void print() override{
    cout << "[ " << nm << " = ";
    val->print();
    cout<<"]";
  }
Value* codegen() override;
};

    
class varGetNode : public astNode{
public:
string nm;
 varGetNode(string n ){
   nm = n;
 }
  lType* exec(symbolTable st) override{    
    return st.get(nm);
    return new lNone();
  }
  void print() override{
    cout << "{" << nm << "}";
  }
  Value* codegen() override;
};

class ifNode: public astNode{
public:
  astNode* blk;
  astNode* cnd;
  astNode* ebk;
  bool els;
  bool el = false;
  vector<astNode*> elb;
  vector<astNode*> elc;
  
  ifNode(astNode* c, astNode* b){
    blk = move(b);
    cnd = move(c);
    ebk = nullptr;
    els = false;
  }
  void addElse(astNode* eb){
    ebk = move(eb);
    els = true;
  }
  void addElif(vector<astNode*> eb,
        vector<astNode*> ec){
    elb = eb;
    elc = ec;
    el = true;
  }
  void print() override{
    cout << "IF (";
    cnd->print();
    cout << ") ";
    blk->print();
    int i = 0;
    if (el){
      for( astNode* c:elc){
        c->print();
        cout <<" ";
        elb[i]->print();
        i++;

      }
    }
    if (els){
      cout << "ELSE";
      ebk->print();
    }
  }
  lType* exec(symbolTable st) override{
    if (cnd->exec(st)->truthy()){
      blk->exec(st);
      return new lNone;
    }
    if (el){
     for (int i=0;i<elc.size();i++){
       if (elc[i]->exec(st)->truthy()){
         elb[i]->exec(st);
         return new lNone;
        }
      } 
    }
    if (els){
      ebk->exec(st);
      return new lNone;
    }
    return new lNone;
  }
Value* codegen() override;
};

class whileNode: public astNode{
  public:
  astNode* bk;
  astNode* cnd;
  whileNode(astNode* b, astNode* c){
    bk = move(b);
    cnd = move(c);
  }
  void print() override{
    cout << "WHILE ";
    cnd->print();
    cout << " " ;
    bk->print();
    cout<<endl;
    string _;
  }
  lType* exec(symbolTable st) override{
    string _;
    while (1){
      lType* c = cnd->exec(st);
      if (!c->truthy()) { break;}
      bk->exec(st);
    }
    return new lNone;
  }
Value* codegen() override;
};

class forNode : public astNode{
  public:
  astNode* init;
  astNode* cnd;
  astNode* inc;
  astNode* blk;
  forNode(astNode* i,astNode* cn, astNode* ic,astNode* bk){
    init = move(i);
    cnd = move(cn);
    inc = move(ic);
    blk = move(bk);
  }
  void print() override{
    cout << "FOR( ";
    init->print();
    cout << ", ";
    cnd->print();
    cout << ", ";
    blk->print();
    cout << " )";
  }
  lType* exec(symbolTable st) override{
    init->exec(st);
    while (true){
      lType* c = cnd->exec(st);
      if (!c->truthy()) { break; }
      blk->exec(st);
      inc->exec(st);
    }
    return new lNone;
  }
Value* codegen() override;
};

class fnDefNode : public astNode{
  public:
  astNode* blk;
  string nm;
  vector<string> args;
  astNode* ret;
  fnDefNode(string n, vector<string> a,astNode* bk){
    blk = move(bk);
    nm = n;
    args = a;
  }
  void addRet(astNode* r){
    ret = move(r);
  }
  void print() override{
    cout << "FNDEF "<<nm << "( ";
    for(string arg:args){
      cout << arg << " ";
    }
    cout << " )";
    cout << " -> ";
    blk->print();
  }
  lType* exec(symbolTable st) override{
    lType* fn = new fnType(move(blk),args,nm);
    st.set(nm,fn);
    return new lNone;
  }
Value* codegen() override;
};

class fnCallNode : public astNode{
  public:
  astNode* cl;
  string nm;
  vector<astNode*> args;
  fnCallNode(astNode* c,vector<astNode*> arg,string n){
    args = arg;
    cl = c;
    nm = n;
  }
  void print() override{
    cout << "FNCALL " << nm;
    cout << " (";
    for(astNode* a:args){
      a->print();
      cout<<", ";
    }
    cout << ")";
  }
  lType* exec(symbolTable st) override{
    lType* fn = cl->exec(st);
    vector<lType*>xrg;
    for (auto* arg:args){
      xrg.push_back(arg->exec(st));
    }
    return fn->call(xrg,st);
    
  }
Value* codegen() override;
};

class retNode : public astNode{
  public:
  astNode* nd;
  retNode(astNode* n){
    nd = move(n);
  }
  void print() override{
    cout << "RETURN ";
    nd->print();
  }
  lType* exec(symbolTable st) override{
    lType* r=  nd->exec(st);
    return r;
  }
  bool isRet() override{
    return true;
  }
Value* codegen() override;
};

class propGetNode : public astNode{
  public:
  string prop;
  astNode* nd;
  propGetNode(astNode* n, string p){
    prop = p;
    nd = move(n);
  }
  void print() override{
    nd->print();
    cout<<" . " << prop;
  }
  lType* exec(symbolTable st) override{
    return nd->exec(st)->getProp(prop);
  }
Value* codegen() override;
};
class listNode : public astNode{
  public:
  vector<astNode*> lv;
  listNode(vector<astNode*> l){
    lv = l;
  }
  void print() override {
    cout << "[";
    for(astNode* l:lv){
      l->print();
      cout << ", ";
    }
    cout << "]";
  }
  lType* exec(symbolTable st) override {
    vector<lType*> v;
    for (astNode* l : lv){
      v.push_back(l->exec(st));
    }
    return new listType(v);
  }
Value* codegen() override;
};

class fnProtoNode : public astNode{
  public:
  string nm;
  vector<string> args;
  fnProtoNode(string n,vector<string>a){
    nm = n;
    args = a;
  }
  void print() override {
    cout << "PROTO " << nm << "(";
    for( string n : args){
      cout << n << ", ";
    }
    cout << ")";
  }
  lType* exec(symbolTable st) override {
    error("no exec");
    return nullptr;
  }
Function* codegen() override ;
};


class TypVarSetNode : public astNode{
  public:
  string nm;
  astNode* val;
  string typ;
  
  TypVarSetNode(string n ,astNode* vl,string t){
     nm = n;
     val = move(vl);
     typ = t;
     if (t != "str" && t != "double"){
       error("unknown type " + t);
     }
   }
  lType* exec(symbolTable st) override{
      lType* l = val->exec(st);
      st.set(nm,move(l));
      return new lNone();
    }
    void print() override{
      cout << "[ " << nm << " = ";
      val->print();
      cout << " : " << typ <<" ]";
    }
  Value* codegen() override;
};


#endif
