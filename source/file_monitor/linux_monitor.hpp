#pragma once
#include "monitor.hpp"
#include <sys/inotify.h>

namespace file_monitor
{
class linux_monitor : public monitor
{
public:
  explicit linux_monitor(path_t const& base_path);
  ~linux_monitor() override;

  path_t base_path() const override;
  void poll(change_event_t const& consumer) override;

private:
  struct watch_t
  {
    int id;
    path_t base_path;
  };

  void process_events(change_event_t const&);
  void process_event(std::vector<path_t>& changes, inotify_event const* event);
  void create_watches(path_t const& root);
  watch_t const& find_watch(int id) const;

  void submit_changes(change_event_t const& consumer,
                      std::vector<path_t> const& changed_files) const;

  void read_changes(std::vector<path_t>& changed_files, std::array<char, 4096>& events);

  int m_handle = -1;
  int m_base_watch_id = -1;
  path_t m_base_path;

  std::vector<watch_t> m_watches;
};
} // namespace file_monitor
