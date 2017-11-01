#pragma once
#include "monitor.hpp"
#include <sys/inotify.h>

namespace file_monitor
{
class linux_monitor : public monitor
{
public:
  linux_monitor();
  ~linux_monitor() override;

  void stop() override;
  void start(const path_t& base_path) override;
  void poll(const change_event_t& consumer) override;

private:
  // \invariant - we need to have a valid inotify instance
  int m_inotify_instance = -1;

  struct watch_t
  {
    int watch_id = -1;
    path_t base_path;
    std::vector<watch_t> sub_watches;
  };

  watch_t m_base_watch;
};
}
