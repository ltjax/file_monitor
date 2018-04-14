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

  path_t base_path() const override;
  void stop() override;
  void start(const path_t& base_path) override;
  void poll(const change_event_t& consumer) override;

private:
  struct watch_t
  {
    int id;
    path_t base_path;
  };

  // \invariant - we need to have a valid inotify instance
  void process_events(change_event_t const&);
  void create_watches(path_t const& root);
  watch_t const& find_watch(int id) const;

  int m_inotify_instance = -1;
  int m_base_watch_id = -1;
  path_t m_base_path;

  std::vector<watch_t> m_watches;
};
}
