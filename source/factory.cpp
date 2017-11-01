#include "factory.hpp"

#ifdef FILE_MONITOR_PLATFORM_MACOS
#include "mac_monitor.hpp"
#endif

#ifdef FILE_MONITOR_PLATFORM_WIN32
#include "win_monitor.hpp"
#endif

#ifdef FILE_MONITOR_PLATFORM_LINUX
#include "linux_monitor.hpp"
#endif


std::shared_ptr<file_monitor::monitor> file_monitor::make_monitor()
{
#if defined(FILE_MONITOR_PLATFORM_MACOS)
    return std::make_shared<mac_monitor>();
#elif defined(FILE_MONITOR_PLATFORM_WIN32)
    return std::make_shared<win_monitor>();
#else
    return std::make_shared<linux_monitor>();
#endif
}
