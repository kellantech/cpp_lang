#include <iostream>
#include <map>
#include <vector>

using namespace std;

#include "utils.hpp"

class lType;
class astNode;
class symbolTable;

#include "symbolTable.hpp"
#include "lType_base.hpp"
#include "astNode_base.hpp"

void notImpl(string);

class stringType : public lType{
  public:
  stringType(string v){ sval = v; typ = "STR"; }
  string sget() override {return sval;} 
  void print() override{
    cout << '"' << sval << '"';
  }
  lType* add(lType *r) override{
    if (r->typ == "STR"){
      return new stringType(sval + r->sget());
    }
    else {
      notImpl("add");
    }
  }
  bool truthy() override{
    return sval.length() > 0;
  }
};

class numberType: public lType{
  public:
  int ival;
  numberType(int v){ival = v; typ = "INT";}
  numberType() {error("empty constructor not allowed");}
  int iget() override{
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
      return new stringType(string(r->sget(),ival));
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
    cout << "**";
    rs->print();
    cout<<endl;
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
    return new numberType(val);
  }
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
    string _;
    while (1){
      lType* c = cnd->exec(st);
      c->print();
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
    return new None;
  }
};

class fnCallNode : public astNode{
  public:
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
    return fn->call(xrg,st);
    
  }
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
  vector<string> KWDS {"var","if","else","elif","while","for","fn","ret"};
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
      else if (cur == '.'){
        tks.push_back(token(tt("DOT")));
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
    astNode* ret;
    
    if (cur.type == tt("INT")){
      string tmp = cur.val;
      next();
      ret = new intNode(stoi(tmp));
    }
      
    else if (cur.type == tt("SUB")){
      next();
      astNode* r = value();
      ret = new unOpNode(tt("SUB"),move(r));  
    }
    else if (cur.type == tt("ID")){
      string n = cur.val;
      next();
      if (cur.type == tt("LP")){
        next();
        vector<astNode*> arg;
        while (cur.type != tt("RP")){
          arg.push_back(logical_expr());
          if (cur.type == tt("CMA")){ next(); }
        }
        ret = new fnCallNode(n,arg);
      }
      else { ret = new varGetNode(n); }
    }
    else if (cur.type == tt("LP")){
      next();
      ret = logical_expr();
      next();
    }
    else{
      error("expcted int");
      return nullptr;
    }
    if (cur.type == tt("DOT")){
      next();
      if (cur.type == tt("ID")){
        string p = cur.val;
        next();
        return new propGetNode(move(ret),p);
      }
    }
    return move(ret);
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
        astNode* blk = block(true);
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
  astNode* block(bool isFn=false){
    vector<astNode*> op;
    if (cur.type != tt("LCB")){ error("expected '{'"); }
    else{
      next();
      while (1){
        if (cur.type == tt("KWD") && cur.val == "ret"){
          next();
          astNode* nd = logical_expr();
          op.push_back(new retNode(move(nd)));
        }
        else{
          astNode* cn = logical_expr();
          op.push_back(move(cn));
        }
        if (cur.type == tt("RCB")){ break;};
      }
    }
    next();
    return new blockNode(op,isFn);
  }
  astNode* binOp(int tks[],string fn,int sz){
    
    astNode* node;
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
