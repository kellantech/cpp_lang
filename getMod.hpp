#include "stdlib/math.hpp"


void getMod(string nm,symbolTable& st){
  if (nm == "math"){
    MATH_add(st);
  }
  else {
    error("no module named "+nm);
  }
}
