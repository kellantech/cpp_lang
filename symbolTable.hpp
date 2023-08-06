#ifndef _ST_H
#define _ST_H

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

#endif
