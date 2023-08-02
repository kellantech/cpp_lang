#include <iostream>
#include <map>
#include <vector>

using namespace std;


template <typename T>
bool findvec(vector<T> vec, T e) {
  for (T el : vec){
    if ( e == el){  return true; }
  }
  return false;
}


template <typename T>
bool findary(T ary [], T e, int s) {
  for (int i =0;i<s;i++){
    if ( e == ary[i]){return true;}
  }
  return false;

}

void error(string msg){
  cout << "error: " << msg << endl;
  exit(1);
}
int tt(string s){
  
  map<string,int> tt_map {
    {"ADD",1},{"SUB",2},{"MUL",3},
    {"DIV",4},{"INT",5},{"LP",6},
    {"RP",7 },{"KWD",8},{"ID",9},
    {"EQL",10},{"EQ",11},{"LCB",12},
    {"RCB",13},{"NE",14},{"GT",15},
    {"LT",16},{"AND",17},{"OR",18},
    {"CMA",19},
  
    {"NONE",100},{"NUL",101}
  };
  
  return tt_map[s];
}

class lType;
class astNode;
class symbolTable;



class symbolTable{
  public:
  map<string,lType*>& tbl;
  symbolTable* parent;
  bool p = false;
  symbolTable(map<string,lType*>& t):tbl(t){
    
  }
  void setParent(symbolTable* par){
    parent = move(par);
    p = true;
  }
  lType* get(string nm){
    if(tbl.find(nm) == tbl.end()){
      if (p){ 
        return parent->get(nm);
      }
      else{
        error("name not found");
        return nullptr;
      }
    }
    else {
      return move(tbl[nm]);
    }
  }
  void set(string n,lType* v){
    tbl[n] = move(v);
  }
};


class lType{
  public:
  int ival;
  virtual int iget(){error("virtual");}
  virtual void print(){cout << ":[";};
  virtual lType* add(lType* r) {
    error("add not implemented");
  }
   virtual lType* sub(lType* r) {
    error("sub not implemented");
  }
  virtual lType* mul(lType* r) {
    error("mul not implemented");
  }
  virtual lType* div(lType* r) {
    error("div not implemented");
  }
  virtual lType* eql(lType* r) {
    error("eql not implemented");
  }
  virtual lType* neq(lType* r) {
    error("neq not implemented");
  }
  virtual lType* gt(lType* r) {
    error("gt not implemented");
  }
  virtual lType* lt(lType* r) {
    error("lt not implemented");
  }
  virtual bool truthy() {
    error("truthy not implemented");
  }
  virtual lType* call(vector<lType*> v,symbolTable st) {
    error("not callable");
  }
};

class astNode{
  public:
  virtual lType* exec( symbolTable st ){
    cout<<":("<<endl;
    error("...");
  }
  virtual void print(){
    cout << ":(p"<<endl;
  }
};


class numberType: public lType{
  public:
  int ival;
  numberType(int v){ival = v;}
  numberType() {error("empty constructor not allowed");}
  int iget() override{
    return ival;
  }  
void print() override{
    cout << ival << "i";
  }
  lType* add(lType* r) override{
    
    return new numberType(ival + r->iget());
  }
  lType* sub(lType* r) override{
    return new numberType(ival - r->iget());
  }
  lType* mul(lType* r) override{
    return new numberType(ival * r->iget());
  }
  lType* div(lType* r) override{
    return new numberType(ival / r->iget());
  }
  lType* eql(lType* r) override{
    return new numberType((int)(ival == r->iget()));
  }
  lType* neq(lType* r) override{
    return new numberType((int)(ival != r->iget()));
  }
  lType* gt(lType* r) override{
    cout << ival << r->iget() << "!!!" << endl;
    return new numberType((int)(ival > r->iget()));
  }
  lType* lt(lType* r) override{
    return new numberType((int)(ival < r->iget()));
  }
  bool truthy() override{
    return (ival != 0);
  }
  
};

class None : public lType{
  void print() override{
    cout<<"null";  
  }
  bool truthy() override{
    return new numberType(0);
  }
};

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
    map<string,lType*>mp;
    symbolTable nst(mp);
    nst.setParent(&st);
    for(int i = 0; i < v.size(); i++){
      string n = argn[i];
      lType* nd = move(v[i]);
      nst.set(n,move(nd));
    }
    lType* rs = blk->exec(nst);
    for (auto& [key,val]:mp){
      delete val;
    }
    return rs;
  }
};


class intNode : public astNode{
  public:
  int val;
  intNode(int v){val = v;}
  void print() override {
    cout << val << " ";
  }
  lType* exec(symbolTable st) override{
    cout << "IT" << val <<endl;
    return new numberType(val);
  }
  
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
};

class blockNode : public astNode{
public:
  vector<astNode*> ops;
  blockNode( vector<astNode*> op){
    for(astNode* el:op){
      ops.push_back(move(el));
    }
  }
  lType * exec(symbolTable st) override{
    for(astNode* op : ops){
      op->exec(st);
    }
    return new None();
  }
  void print() override{
  cout<<"{";
    for(auto* i:ops){
      i->print();
    }
    cout<<"}";
  }
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
    return new None();
  }
  void print() override{
    cout << "[ " << nm << " = ";
    val->print();
    cout<<"]";
  }
};

    
class varGetNode : public astNode{
public:
string nm;
 varGetNode(string n ){
   nm = n;
 }
  lType* exec(symbolTable st) override{    
    return st.get(nm);
    return new None();
  }
  void print() override{
    cout << "{" << nm << "}";
  }
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
      return new None;
    }
    if (el){
     for (int i=0;i<elc.size();i++){
       if (elc[i]->exec(st)->truthy()){
         elb[i]->exec(st);
         return new None;
        }
      } 
    }
    if (els){
      ebk->exec(st);
      return new None;
    }
    return new None;
  }
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
   // cin>>_;
  }
  lType* exec(symbolTable st) override{
    cout << "W!"<<endl;
    string _;
    while (1){
      lType* c = cnd->exec(st);
      c->print();
      cout<<"%%^"<<endl;
      if (!c->truthy()) { break;}
      bk->exec(st);
      //cin>>_;
    }
    return new None;
  }

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
    return new None;
  }
};

class fnDefNode : public astNode{
  public:
  astNode* blk;
  string nm;
  vector<string> args;
  fnDefNode(string n, vector<string> a,astNode* bk){
    blk = move(bk);
    nm = n;
    args = a;
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
    return new None;
  }
};

class fnCallNode : public astNode{
  string nm;
  vector<astNode*> args;
  fnCallNode(string n,vector<astNode*> arg){
    args = arg;
    nm = n;
  }
  void print() override{
    cout << "FNCALL "<< nm << " (";
    for(astNode* a:args){
      a->print();
      cout<<", ";
    }
    cout << ")";
  }
  lType* exec(symbolTable st) override{
    lType* fn = st.get(nm);
    vector<lType*>xrg;
    for (auto* arg:args){
      xrg.push_back(arg->exec(st));
    }
    fn->call(xrg,st);
  }
};

class token{
public:
  int type;
  string val = "";
  token(){
    type = -1;
  }
  token(int typ){
    type = typ;
  }
  token(int typ,string vl){
    type = typ;
    val = vl;
  }
  void print(){
    if (val == ""){
      cout << "(" << type << ")";
    } 
    else {
      cout << "(" << type << ":" << val << ")";
    }
  }
};


// class lexres {
//   public:
//   vector<token> toks;
//   string error;
//   lexres(vector<token> tok):toks(tok){}
//   lexres(vector<token> tok,string er):toks(tok),error(er){}

// };


class lexer{
public:
  string txt;
  int ind = -1;
  char cur = (char)2;
  vector<string> KWDS {"var","if","else","elif","while","for","fn"};
  lexer(string t){
    txt = t;
    next();
  }
  void next(){
    ind++;
    if(ind >= txt.length()){cur = (char)2;}
    else { cur = txt[ind]; }
  }
  vector<token> gen_toks(){
    vector<token> tks;
    while(1){
      char c = cur;
      if (cur == '+'){
        tks.push_back(token(tt("ADD")));
        next();
      }
      else if (cur == '-'){
        tks.push_back(token(tt("SUB")));
        next();
      }
      else if (cur == '*'){
        tks.push_back(token(tt("MUL")));
        next();
      }
      else if (cur == '/'){
        tks.push_back(token(tt("DIV")));
        next();
      }
      else if (cur == '('){
        tks.push_back(token(tt("LP")));
        next();
      }
      else if (cur == ')'){
        tks.push_back(token(tt("RP")));
        next();
      }
      else if (cur == '{'){
        tks.push_back(token(tt("LCB")));
        next();
      }
      else if (cur == '}'){
        tks.push_back(token(tt("RCB")));
        next();
      }
      else if (cur == '!'){
        next();
        if (cur == '='){
          tks.push_back(token(tt("NE")));
          next();
        }
        else {
          error("unknown token '!'");
        }
      }
      else if (cur == '>'){
        tks.push_back(token(tt("GT")));
        next();
      }
      else if (cur == '<'){
        tks.push_back(token(tt("LT")));
        next();
      }
      else if (cur == '&'){
        tks.push_back(token(tt("AND")));
        next();
      }
      else if (cur == '|'){
        tks.push_back(token(tt("OR")));
        next();
      }
      else if (cur == ','){
        tks.push_back(token(tt("CMA")));
        next();
      }
      else if (cur == '='){
        next();
        if(cur == '='){
          tks.push_back(token(tt("EQ")));
          next();
          continue;
        }
        else{
          tks.push_back(token(tt("EQL")));
        }
      }
      
      else if (cur == ' ' || cur == '\n') { 
        next(); 
      }
      else if (cur == (char) 2) { 
        break;
      }
      else if (isalpha(cur)){
        string r;
        while(isalpha(cur) || isdigit(cur)){
          r += string{cur};
          next();
        }
        if ( findvec<string>(KWDS,r)) {
          tks.push_back(token(tt("KWD"),r));
        }
        else {
          tks.push_back(token(tt("ID"),r));
        }
      }
      else if (isdigit(cur)){
        string r;
        while (isdigit(cur)){
          r += string{cur};
          next();
        }
        tks.push_back(token(tt("INT"),r));
      }
      else{
        error("unknown character '" + string{cur} + "'");
      }
    }
    return tks;
  }
  
};


class parser{
public:
  vector<token> tkn;
  token cur ;
  int ind = -1;
  parser(vector<token>& t){
    tkn = t;
    next();
  }
  void next(){
    ind++;
    if (ind >= tkn.size()){
      cur =  token(tt("NONE"));
    }
    else {
      cur = tkn[ind];
    }
  }
  
  astNode* value(){
    if (cur.type == tt("INT")){
      string tmp = cur.val;
      next();
      return new intNode(stoi(tmp));
    }
      
    else if (cur.type == tt("SUB")){
      next();
      astNode* r = value();
      return new unOpNode(tt("SUB"),move(r));  
    }
    else if (cur.type == tt("ID")){
      string n = cur.val;
      next();
      return new varGetNode(n);
    }
    else if (cur.type == tt("LP")){
      next();
      astNode* p = logical_expr();
      next();
      return p;
      
    }
    else{
      error("expcted int");
      return nullptr;
    }
  }
  astNode* term (){
    int op[] = {tt("MUL"),tt("DIV")};
    return binOp(op, "value",2);
  }
  astNode* cmp(){
    int op[] = {tt("EQ"),tt("NE"),tt("LT"),tt("GT")};
    return binOp(op,"expr",4);
  }
  astNode* logical_expr(){
    if (cur.type == tt("KWD") && cur.val == "var"){
      next();
      string nm;
      if (cur.type == tt("ID")){
        nm = cur.val;
      }
      else{
        error("expected identifier");
      }
      next();
      if (cur.type != tt("EQL")){
        error("expected '='");
      }
      next();
      astNode* a = logical_expr();
      return new varSetNode(nm,a);
    }
    if (cur.type == tt("KWD") && cur.val == "if"){
      next();
      astNode* cnd = logical_expr();
      cur.print();
      cout << "!&^"<<endl;
      astNode* bl = block();
      vector<astNode*> ecv;
      vector<astNode*> ebv;
      while (cur.type == tt("KWD") && cur.val == "elif"){
        next();
        astNode* e = logical_expr();
        ecv.push_back(e);
        astNode* b = block();
        ebv.push_back(b);
      }
      if (cur.type == tt("KWD") && cur.val == "else"){
        next();
        astNode* eb = block();
        cout<<"ELS"<<endl;
        ifNode* n =  new ifNode(move(cnd),move(bl));
        n->addElse(move(eb));
        n->addElif(ebv,ecv);
        return move(n);
      }
      else{
        ifNode* n = new ifNode(move(cnd),move(bl));
        n->addElif(ebv,ecv); 
        return move(n);
      }
    }
    else if ( cur.type == tt("KWD") && cur.val == "while"){
      next();
      astNode* c = logical_expr();
      astNode* b = block();
      return new whileNode(b,c);
    }
    else if ( cur.type == tt("KWD") && cur.val == "for" ){
      next();
      astNode* i;
      astNode* c;
      astNode* ic;
      astNode* b;
      if (cur.type == tt("LP")){
        next();
        i = logical_expr();
        if (cur.type == tt("CMA")){
          next();
          c = logical_expr();
          if (cur.type == tt("CMA")){
            next();
            ic = logical_expr();
            if (cur.type == tt("RP")){
              next();
              b = block();
              return new forNode(i,c,ic,b);
            }
            else{
                error("expected ')'");
            }
          }
          else {
           error("expected ','");
          }    
        }
        else {
          error("expected ','");
        }
      }
      else{
        error ("expected '('");
      }
    }
    else if (cur.type == tt("KWD") && cur.val == "fn"){
      next();
      string nm = cur.val;
      next();
      vector<string> arg;
      if (cur.type == tt("LP")){
        next();
        while (cur.type != tt("RP")){
          arg.push_back(cur.val);
          next();
          if (cur.type == tt("CMA")) { next(); }
        }
        next();
        astNode* blk = block();
        return new fnDefNode(nm,arg,blk);
      }
      else {
        error("expected '('");
      }
    }
    int op[] = {tt("AND"),tt("OR")};
    return binOp(op,"cmp",2);
  }
  astNode* expr(){
    int op[] = {tt("ADD"),tt("SUB")};
    return binOp(op,"term",2);
  }
  astNode* block(){
    vector<astNode*> op;
    if (cur.type != tt("LCB")){ error("expected '{'"); }
    else{
      next();
        while (1){
        astNode* cn = logical_expr();
        op.push_back(move(cn));
        cout<<cur.type<<endl;
        if (cur.type == tt("RCB")){ break;};
      }
    }
    next();
    return new blockNode(op);
  }
  astNode* binOp(int tks[],string fn,int sz){
    
    astNode* node;
    cout<<fn<< " { " << endl;
    if (fn == "value"){ node = value(); }
    else if (fn == "term") { node = term(); }
    else if (fn == "expr") { node = expr(); }
    else if (fn == "cmp") { node = cmp(); }
    else { error("unknown function : parser : binOP"); }
    
    while (1){
      if (cur.type == -1 || ! findary<int>(tks, cur.type, sz)) { break; }
      token ot (cur.type,cur.val.c_str());
      next();
      astNode* r ;
      if (fn == "value"){ r= value(); }
      else if (fn == "term") { r = term(); }
      else if (fn == "expr") { r = expr(); }
      else if (fn == "cmp") { r = cmp(); }
      node = new binopNode(node,r,ot.type);
    }
    cout << "}"<<endl;
    return node;
  }
};

int main() {
  map<string,lType*> m;
  symbolTable gst(m);
string inp;
  while (1){
    inp = "";
    cout<<"lexer parser inter0> ";
    getline(cin,inp);
    cout<<inp<<"!"<<endl;
    lexer ls (inp);
    vector<token>r = ls.gen_toks();
    for(auto e:r) e.print();
    
    cout<<endl;
    parser p (r);
    astNode* exp = p.logical_expr();
    exp -> print();
    cout<<endl;
    exp -> exec(gst) -> print();
    cout<<endl;
    ///gst.get("x") -> print();
    }
}
