#include <iostream>
#include <map>
#include <vector>
#include <functional>
using namespace std;

#include "utils.hpp"

class lType;
class astNode;
class symbolTable;


#include "symbolTable.hpp"
#include "lType_base.hpp"
#include "astNode_base.hpp"
#include "lType.hpp"
#include "astNode.hpp"



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
      else if (cur == '"'  || cur == '\''){
        char p = cur;
        string r;
        next();
        while(cur != p){
          r += string{cur};
          next();
        }
        next();
        tks.push_back(token(tt("STR"),r));
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
    else if (cur.type == tt("STR")){
      string tmp = cur.val;
      
      next();
      ret = new stringNode(tmp);
    }
      
    else if (cur.type == tt("SUB")){
      next();
      astNode* r = value();
      ret = new unOpNode(tt("SUB"),move(r));  
    }
    else if (cur.type == tt("ID")){
      string n = cur.val;
      next();
      
      ret = new varGetNode(n); 
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
    else if (cur.type == tt("LP")){
        next();
        vector<astNode*> arg;
        while (cur.type != tt("RP")){
          arg.push_back(logical_expr());
          if (cur.type == tt("CMA")){ next(); }
        }
        ret = new fnCallNode(ret,arg);
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
