
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
      else if (cur == '['){
        tks.push_back(token(tt("LB")));
        next();
      }
      else if (cur == ']'){
        tks.push_back(token(tt("RB")));
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
        while (isdigit(cur) || cur == '.'){
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
