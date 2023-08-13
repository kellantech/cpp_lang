#include "stdlib/math.hpp"
#include "stdlib/rand.hpp"



void getMod(string nm,symbolTable& st){
  if (nm == "math"){
    MATH_add(st);
  }
  else if (nm == "random"){
    RAND_add(st);
  }
  else {
    error("no module named "+nm);
  }
}
