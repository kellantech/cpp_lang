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



void MATH_add(symbolTable& st){
  
  st.set("fact", new builtInFn(1,&fact));
  st.set("sin", new builtInFn(1,&lsin));
}
