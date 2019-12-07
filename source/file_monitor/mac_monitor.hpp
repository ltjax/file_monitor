#pragma once

#include "monitor.hpp"
#include <CoreServices/CoreServices.h>
#include <map>
#include <mutex>
#include <thread>

namespace file_monitor
{
class mac_monitor : public monitor
{
public:
  explicit mac_monitor(path_t const& where);
  ~mac_monitor() override;

  path_t base_path() const override;
  void poll(change_event_t const& consumer) override;

private:
  void run(FSEventStreamRef stream);

  using hashcode_t = std::uint32_t;

  struct detail;

  hashcode_t hash_file(path_t const& filepath);
  void path_changed(path_t const& where);
  path_t relative_path(path_t const& File);
  void hash_files_in(path_t const& root);

  path_t m_base_path;
  std::thread m_event_thread;
  CFRunLoopRef m_run_loop = nullptr;
  std::map<path_t, hashcode_t> m_file_hash_for;

  std::mutex m_file_list_mutex;
  file_list_t m_changed;
  std::atomic_bool m_keep_running{ false };
  CFRunLoopSourceRef m_stop_source = nullptr;
};
}