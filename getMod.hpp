lType* test(vector<lType*> inp){
  
  cout << ":) !" << endl;
  return new None;
}

void getMod(string nm,symbolTable &st){
  st.set("test", new builtInFn(0,&test));
}
