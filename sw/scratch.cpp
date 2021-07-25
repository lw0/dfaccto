#include <iostream>

#include "bitvector.hpp"

int main(int argc, char ** argv)
{
  sim::BitVector vec(0);
  vec.fromHex("DEADB--F");
  std::cout << "1: " << vec.toBin() << std::endl;
  vec.fromBin("101001010-0-1100----1111");
  std::cout << "2: " << vec.toHex() << std::endl;
  vec.fromStr("Hello World!");
  std::cout << "3: " << vec.toHex() << std::endl;
  sim::BitVector vec2 = vec.slice(4, 64);
  std::cout << "4: " << vec2.toStr() << std::endl;
  sim::BitVector vec3 = vec.slice(8, 64);
  std::cout << "5: " << vec3.toStr() << std::endl;
}
