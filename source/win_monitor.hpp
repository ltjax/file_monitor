#pragma once
#include "monitor.hpp"

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#define NOMINMAX

#include <boost/algorithm/string/predicate.hpp>
#include <boost/utility.hpp>
#include <windows.h>
#include <chrono>

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
    void poll(change_event_t const& consumer) override;
private:
    bool listen();
    void add_path(char const* filename);

    using clock_t = std::chrono::high_resolution_clock;
    std::vector<char> mResultBuffer;
    HANDLE DirectoryHandle= INVALID_HANDLE_VALUE;
    HANDLE NotifyEvent = nullptr;
    OVERLAPPED OverlappedIO = {};

    bool CountdownStarted = false;
    clock_t::time_point CountdownTime;
    clock_t::duration CountdownLength;
    path_t BasePath;

    std::vector<path_t> FilesChanged;
};
}