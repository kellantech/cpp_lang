
class lexer{
public:
  string txt;
  int ind = -1;
  int ln = 1;
  char cur = (char)2;
  vector<string> KWDS {"var","if","else","elif","while","for","fn","ret","proto"};
  lexer(string t){
    txt = t;
    next();
  }
  void next(){
    ind++;
    if (cur == '\n'){ln++;}
    if(ind >= txt.length()){cur = (char)2;}
    else { cur = txt[ind]; }
    
  }


  vector<token> gen_toks(){
    vector<token> tks;
    while(1){
      char c = cur;
      if (cur == '+'){
        tks.push_back(token(tt("ADD"),pos(ln)));
        next();
      }
      else if (cur == '-'){
        tks.push_back(token(tt("SUB"),pos(ln)));
        next();
      }
      else if (cur == '*'){
        tks.push_back(token(tt("MUL"),pos(ln)));
        next();
      }
      else if (cur == '/'){
        tks.push_back(token(tt("DIV"),pos(ln)));
        next();
      }
      else if (cur == '('){
        tks.push_back(token(tt("LP"),pos(ln)));
        next();
      }
      else if (cur == ')'){
        tks.push_back(token(tt("RP"),pos(ln)));
        next();
      }
      else if (cur == '['){
        tks.push_back(token(tt("LB"),pos(ln)));
        next();
      }
      else if (cur == ']'){
        tks.push_back(token(tt("RB"),pos(ln)));
        next();
      }
      else if (cur == '{'){
        tks.push_back(token(tt("LCB"),pos(ln)));
        next();
      }
      else if (cur == '}'){
        tks.push_back(token(tt("RCB"),pos(ln)));
        next();
      }
      else if (cur == '!'){
        next();
        if (cur == '='){
          tks.push_back(token(tt("NE"),pos(ln)));
          next();
        }
        else {
          error("unknown token '!'",pos(ln));
        }
      }
      else if (cur == '>'){
        tks.push_back(token(tt("GT"),pos(ln)));
        next();
      }
      else if (cur == '<'){
        tks.push_back(token(tt("LT"),pos(ln)));
        next();
      }
      else if (cur == '&'){
        tks.push_back(token(tt("AND"),pos(ln)));
        next();
      }
      else if (cur == '|'){
        tks.push_back(token(tt("OR"),pos(ln)));
        next();
      }
      else if (cur == ','){
        tks.push_back(token(tt("CMA"),pos(ln)));
        next();
      }
      else if (cur == '.'){
        tks.push_back(token(tt("DOT"),pos(ln)));
        next();
      }
      else if (cur == '='){
        next();
        if(cur == '='){
          tks.push_back(token(tt("EQ"),pos(ln)));
          next();
          continue;
        }
        else{
          tks.push_back(token(tt("EQL"),pos(ln)));
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
        tks.push_back(token(tt("STR"),r,pos(ln)));
      }
      else if (isalpha(cur)){
        string r;
        while(isalpha(cur) || isdigit(cur)){
          r += string{cur};
          next();
        }
        if ( findvec<string>(KWDS,r)) {
          tks.push_back(token(tt("KWD"),r,pos(ln)));
        }
        else {
          tks.push_back(token(tt("ID"),r,pos(ln)));
        }
      }
      else if (isdigit(cur)){
        string r;
        while (isdigit(cur) || cur == '.'){
          r += string{cur};
          next();
        }
        tks.push_back(token(tt("INT"),r,pos(ln)));
      }
      else{
        error("unknown character '" + string{cur} + "'",pos(ln));
      }
    }
    return tks;
  }
  
};
