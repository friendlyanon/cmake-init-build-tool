#include "lib.hpp"

auto get_data() -> std::string;

library::library()
    : data {get_data()}
{
}
