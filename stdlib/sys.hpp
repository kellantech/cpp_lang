namespace SYS{
  string sys_get(string s){
    char fnm[] = "/tmp/fileXXXXXX";
    int _fd = mkstemp(fnm);
    int sc = system((s + " > " + fnm).c_str());
    
    return (!sc)?readFile(fnm):"command failed";
  }
  void sys_exec(string s){
    system(s.c_str());
  }
  void SYS_add(symbolTable& st){
    st.set("shellget",new builtInFn(1,[](vector <lType*> v)->lType*{
      return new stringType(sys_get(v[0]->sget()));
    }));
    st.set("shell",new builtInFn(1,[](vector <lType*> v)->lType*{
      sys_get(v[0]->sget());
      return new None;
    }));
  }
}
