#ifndef LTYPE_H
#define LTYPE_H

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
  virtual lType* getProp(string p) {
    error("no proptery '"+p+"'" );
  }

};
#endif
