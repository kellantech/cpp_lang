class token{
public:
  int type;
  pos p;
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
  token(int typ,pos ps){
    type = typ;
    p = ps;
  }
  token(int typ,string vl,pos ps){
    type = typ;
    val = vl;
    p = ps;
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
