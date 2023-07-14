#include <iostream>

#include "lib.hpp"

auto main() -> int
{
  auto const lib = library {};
  std::cout << "Data from precompiled source:\n" << lib.data << '\n';
  return 0;
}
