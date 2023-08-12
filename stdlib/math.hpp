lType* fact(vector<lType*> v){
  int n = v[0]->iget();
  int r = 1;
  for(;n>0;n--){
    r *= n;
  }
  return new numberType(r);
}
int ifact(int x){
  if (x==0){return 1;}
  else {return x*ifact(x-1);}
}

lType* lsin(vector<lType*> v){
  float x = v[0]->iget();
  float a = fmod(x,3.1415);
  float rs = 0;
  int m1 = -1;
  for (int i=0;i<11;++i){
		rs = rs + ((pow(m1,i) / ifact(2 * i + 1)) * pow(x,2*i+1));
	};
  return new numberType(rs);
}

lType* lcos(vector<lType*> v){
  lType* n = v[0];
  lType* c = ((new numberType(1.5707))->sub(n));
  return lsin(vector<lType*>{c});
}

lType* lexp(vector<lType*> v){
  lType* n = v[0];
  return new numberType(pow(2.71828,n->iget()));
}

lType* lsqrt(vector<lType*> v){
  lType* n = v[0];
  return new numberType(pow(n->iget(),0.5));
}
lType* lhyp(vector<lType*> v){
  lType* a = v[0];
  lType* b = v[1];
  return new numberType(pow(pow(a->iget(),2)+pow(b->iget(),2),0.5));
}





void MATH_add(symbolTable& st){
  
  st.set("fact", new builtInFn(1,&fact));
  st.set("sin", new builtInFn(1,&lsin));
  st.set("cos", new builtInFn(1,&lcos));
  st.set("exp", new builtInFn(1,&lexp));
  st.set("sqrt", new builtInFn(1,&lsqrt));
  st.set("hyp", new builtInFn(2,&lhyp));
  
  st.set("pi", new numberType(3.14159));
  st.set("e", new numberType(2.71828));
}
