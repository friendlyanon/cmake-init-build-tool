#include "lib.hpp"

auto main() -> int
{
  auto const lib = library {};

  return lib.data.find("input=") == 0u ? 0 : 1;
}
