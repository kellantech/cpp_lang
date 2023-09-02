#ifndef LTYPE_H
#define LTYPE_H
class lType{
  public:
  float ival;
  string sval;
  vector<lType*> lval;
  string typ;
  virtual float iget(){error("virtual");return 0;}
  virtual string sget(){error("virtual"); return "";}
  virtual vector<lType*> lget(){error("virtual"); return vector<lType*>{};}
  virtual void print(){cout << ":[";};
  virtual lType* add(lType* r) {
    error("add not implemented");
    return nullptr;
  }
   virtual lType* sub(lType* r) {
    error("sub not implemented");
    return nullptr;
  }
  virtual lType* mul(lType* r) {
    error("mul not implemented");
    return nullptr;
  }
  virtual lType* div(lType* r) {
    error("div not implemented");
    return nullptr;
  }
  virtual lType* eql(lType* r) {
    error("eql not implemented :(");
    return nullptr;
  }
  virtual lType* neq(lType* r) {
    error("neq not implemented");
    return nullptr;
  }
  virtual lType* gt(lType* r) {
    error("gt not implemented");
    return nullptr;
  }
  virtual lType* lt(lType* r) {
    error("lt not implemented");
    return nullptr;
  }
  virtual bool truthy() {
    error("truthy not implemented");
    return false;
  }
  virtual lType* call(vector<lType*> v,symbolTable st) {
    error("not callable");
    return nullptr;
  }
  virtual lType* getProp(string p) {
    error("no property '"+p+"'" );
    return nullptr;
  }

};

#endif
