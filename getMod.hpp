#include "stdlib/math.hpp"
#include "stdlib/rand.hpp"



void getMod(string nm,symbolTable& st){
  if (nm == "math"){
    MATH::MATH_add(st);
  }
  else if (nm == "random"){
    RAND::RAND_add(st);
  }
  else {
    error("no module named "+nm);
  }
}
