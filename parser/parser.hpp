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
      cur =  token(tt("NONE"),cur.p);
    }
    else {
      cur = tkn[ind];
    }
  }
  token look(){
    if (ind >= tkn.size()){
      return token(tt("NONE"),cur.p);
    }
    else {
      return tkn[ind+1];
    }
  }
  astNode* value(){
    astNode* ret;
    string nm;
    
    if (cur.type == tt("INT")){
      string tmp = cur.val;
      next();
      ret = new intNode(stof(tmp));
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
      nm = n;
      
      ret = new varGetNode(n); 
    }
    else if (cur.type == tt("LP")){
      next();
      ret = logical_expr();
      next();
    }
    else if (cur.type == tt("LB")){
      next();
      vector<astNode*> v;
      while (cur.type != tt("RB")){
        
        v.push_back(logical_expr());
        if (cur.type == tt("CMA")){
          next();
        }
        if (cur.type == tt("RB")){
          break;
        }
      }
      next();
      return new listNode(v);
    }

    else{
      if (cur.type == tt("NONE")){
        error("unexpected EOF",cur.p-1);
      }
      
      error("expected int",cur.p-1);
      return nullptr;
    }
    check:;
    if (cur.type == tt("DOT")){
      next();
      if (cur.type == tt("ID")){
        string p = cur.val;
        next();
        ret = new propGetNode(move(ret),p);
        goto check;
      }
    }
    else if (cur.type == tt("LP")){
        next();
        vector<astNode*> arg;
        while (cur.type != tt("RP")){
          arg.push_back(logical_expr());
          if (cur.type == tt("CMA")){ next(); }
        }
        next();
        ret = new fnCallNode(ret,arg,nm);
        goto check;
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
    if ((cur.type == tt("KWD") && cur.val == "var") || (cur.type == tt("ID") && (look().type == tt("EQL") || look().type == tt("CLN")))){
      if (cur.val == "var"){
        next();
      }
      string nm;
      if (cur.type == tt("ID")){
        nm = cur.val;
      }
      else{
        error("expected identifier",cur.p);
      }
      next();
      string typ = "";
      if (cur.type == tt("CLN")){
        next();
        if (cur.val == "str" || cur.val == "double"){
          typ = cur.val;
        }
        else {
          error("unknown type " + cur.val, cur.p);
        }
        next();
      }
      if (cur.type != tt("EQL")){
        error("expected '='",cur.p);
      }
      next();
      astNode* a = logical_expr();
      if (typ != ""){
        return new TypVarSetNode(nm,a,typ);
      }
      else {
       return new varSetNode(nm,a);
      }
    }
    if (cur.type == tt("KWD") && cur.val == "if"){
      next();
      astNode* cnd = logical_expr();
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
                error("expected ')'",cur.p);
            }
          }
          else {
           error("expected ','",cur.p);
          }    
        }
        else {
          error("expected ','",cur.p);
        }
      }
      else{
        error ("expected '('",cur.p);
      }
    }
    else if (cur.type == tt("KWD") && cur.val == "fn"){
      next();
      if (cur.type != tt("ID")){
        error("expected identifier",cur.p);
      }
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
        error("expected '('",cur.p);
      }
    }
    else if (cur.type == tt("KWD") && cur.val == "proto"){
      next();
      if (cur.type != tt("ID")){
        error("expected identifer");
      }
      string n = cur.val;
      next();
      if (cur.type != tt("LP")){
        error("expected '('");
      }
      next();
      vector<string> arg;
      while (cur.type != tt("RP")){
        arg.push_back(cur.val);
        next();
        if (cur.type == tt("CMA")) {
          next(); 
        }
      }
      next();
      return new fnProtoNode(n,arg);
    }
    int op[] = {tt("AND"),tt("OR")};
    astNode* bnop = binOp(op,"cmp",2);
    return bnop;
    ///return new fnDefNode("__anon_expr",vector<string>{},move(bnop));
  }
  astNode* expr(){
    int op[] = {tt("ADD"),tt("SUB")};
    return binOp(op,"term",2);
  }
  astNode* block(bool isFn=false){
    vector<astNode*> op;
    if (cur.type != tt("LCB")){ error("expected '{'",cur.p); }
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
    else { error("unknown function : parser : binOP",cur.p); }
    
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
