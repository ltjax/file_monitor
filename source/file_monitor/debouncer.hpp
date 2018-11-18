#pragma once
#include "monitor.hpp"
#include <chrono>

namespace file_monitor
{
class debouncer : public monitor
{
public:
  using clock_t = std::chrono::high_resolution_clock;
  explicit debouncer(std::unique_ptr<monitor> decorated,
            clock_t::duration countdown_length = std::chrono::milliseconds(500));
  ~debouncer() override;

  path_t base_path() const override;

  void poll(change_event_t const& consumer) override;

private:
  std::unique_ptr<monitor> m_decorated;
  bool m_countdown_started = false;
  clock_t::time_point m_countdown_time;
  clock_t::duration m_countdown_length;
  std::vector<path_t> m_files_changed;
};
}
