#pragma once

#include <string_view>
#include <iostream>

#include "theatre/types.hh"

namespace theatre {

constexpr bool VERBOSE_LOGGING = false;

class Any;
Any RunScript(const std::string_view& script, std::ostream& target = std::cout);

};