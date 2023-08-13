
uint64_t _rand_state[2];

void _rand_init(){
  time_t t = time(NULL);
  _rand_state[0] = (t / 60) % 60;
  _rand_state[1] = t % 60;
}


uint64_t lget_next(){
  uint64_t a = _rand_state[0];
  uint64_t b = _rand_state[1];
  _rand_state[0] = b;
  a ^= a << 23;
  a ^= a >> 18;
  a ^= b;
  a ^= b >> 5;
  _rand_state[1] = a;
  return a+b;
}

void RAND_add(symbolTable& st){
  _rand_init();
  st.set("rand",new builtInFn(1,
      [](vector<lType*> v)->lType*{
    return new numberType(lget_next()  % static_cast<uint64_t>(v[0]->iget()));
  }));
  st.set("randfloat",new builtInFn(0,
      [](vector<lType*> v)->lType*{
    return new numberType((lget_next() % 100) / 100.0f);
  }));
  st.set("seed",new builtInFn(2,
      [](vector<lType*> v)->lType*{
        _rand_state[0] = (uint64_t)v[0]->iget();
        _rand_state[1] = (uint64_t)v[1]->iget();
    return new None;
  }));
}
