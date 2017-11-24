#pragma once

#include <memory>
#include "monitor.hpp"

namespace file_monitor
{
std::shared_ptr<monitor> make_monitor();
}


