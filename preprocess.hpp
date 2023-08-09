#ifndef _PREP_H
#define _PREP_H

string imp(string n){
  regex r{"import \"([a-zA-z]*)\""};
  smatch m;
  regex_search(n,m,r);
  string fc = readFile(m.str(1) + ".?");
  return regex_replace(n,r,fc);
}


#endif
