#include "factory.hpp"
#include "debouncer.hpp"

#ifdef FILE_MONITOR_PLATFORM_MACOS
#include "mac_monitor.hpp"
#endif

#ifdef FILE_MONITOR_PLATFORM_WIN32
#include "win_monitor.hpp"
#endif

#ifdef FILE_MONITOR_PLATFORM_LINUX
#include "linux_monitor.hpp"
#endif

std::unique_ptr<file_monitor::monitor> file_monitor::make_platform_monitor(path_t const& where)
{
#if defined(FILE_MONITOR_PLATFORM_MACOS)
  return std::make_unique<mac_monitor>(where);
#elif defined(FILE_MONITOR_PLATFORM_WIN32)
  return std::make_unique<win_monitor>(where);
#elif defined(FILE_MONITOR_PLATFORM_LINUX)
  return std::make_unique<linux_monitor>(where);
#else
#error No supported platform defined
#endif
}

std::unique_ptr<file_monitor::monitor> file_monitor::make_monitor(path_t const& where)
{
  return std::make_unique<file_monitor::debouncer>(make_platform_monitor(where));
}
