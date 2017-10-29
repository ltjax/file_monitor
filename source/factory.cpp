#include "factory.hpp"

#if defined(__APPLE__)
#include "mac_monitor.hpp"
#endif

std::shared_ptr<file_monitor::monitor> file_monitor::make_monitor()
{
#if defined(__APPLE__)
    return std::make_shared<mac_monitor>();
#else
    return nullptr;
#endif
}
