#pragma once
#include "monitor.hpp"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <boost/algorithm/string/predicate.hpp>
#include <boost/utility.hpp>
#include <chrono>
#include <windows.h>

namespace file_monitor
{
class win_monitor : public monitor
{
public:
    win_monitor();
    ~win_monitor() override;

    // Inherited via monitor
    void stop() override;
    void start(path_t const& base_path) override;
    path_t base_path() const override;
    void poll(change_event_t const& consumer) override;

private:
    bool listen();
    void add_path(path_t new_filename);

    using clock_t = std::chrono::high_resolution_clock;
    std::vector<char> m_result_buffer;
    HANDLE m_directory_handle = INVALID_HANDLE_VALUE;
    HANDLE m_notify_event = nullptr;
    OVERLAPPED m_overlapped_io = {};

    bool m_countdown_started = false;
    clock_t::time_point m_countdown_time;
    clock_t::duration m_countdown_length;
    path_t m_base_path;

    std::vector<path_t> m_files_changed;
};
}