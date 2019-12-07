#pragma once

#include <file_monitor/monitor.hpp>
#include <string>

class scoped_temp_folder
{
public:
  explicit scoped_temp_folder(std::string const& prefix);
  scoped_temp_folder(scoped_temp_folder const&) = delete;
  ~scoped_temp_folder();
  scoped_temp_folder& operator=(scoped_temp_folder const&) = delete;

  operator file_monitor::path_t const&() const
  {
    return get();
  }

  file_monitor::path_t const& get() const
  {
    return m_path;
  }

private:
  file_monitor::path_t m_path;
};
