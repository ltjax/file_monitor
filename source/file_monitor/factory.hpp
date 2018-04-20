#pragma once

#include "monitor.hpp"
#include <memory>

namespace file_monitor
{
/** Generate a default monitor.
    That is currently a platform monitor with a 0.5 second debouncer.
*/
std::shared_ptr<monitor> make_monitor();

/** Generate a platform monitor.
    This monitor will report changes as the OS reports them.
*/
std::shared_ptr<monitor> make_platform_monitor();
}
