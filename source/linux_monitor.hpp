#pragma once
#include "monitor.hpp"

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
};
}
