#pragma once
#include "monitor.hpp"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

namespace file_monitor
{
class win_monitor : public monitor
{
public:
  win_monitor(path_t const& base_path);
  ~win_monitor() override;

  // Inherited via monitor
  path_t base_path() const override;
  void poll(change_event_t const& consumer) override;

private:
  void listen();

  std::vector<char> m_result_buffer;
  HANDLE m_directory_handle = INVALID_HANDLE_VALUE;
  HANDLE m_notify_event = nullptr;
  OVERLAPPED m_overlapped_io = {};

  path_t m_base_path;

  std::vector<path_t> m_files_changed;
};
}
