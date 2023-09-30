#ifndef _PREP_H
#define _PREP_H


tuple<string,vector<string>> imp(string n){
  string n_c = n;
  regex r{"import \"([a-zA-z0-9_\?./]*)\""};
  smatch m;
  string fc;
  vector<string> bn;
  while (regex_search(n,m,r)){
    if (!flexists(m.str(1) + ".?")){
      bn.push_back(m.str(1));
      n_c = regex_replace(n_c,r," ",regex_constants::format_first_only);
      n = m.suffix();
    }
    else {
      fc = readFile(m.str(1) + ".?");
      n_c =  regex_replace(n_c,r,fc,regex_constants::format_first_only);
      n = m.suffix();
    }
  };
  return make_tuple(n_c,bn);
}


#endif
