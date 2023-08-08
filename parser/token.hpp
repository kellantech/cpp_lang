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
