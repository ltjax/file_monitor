#pragma once

#include <boost/filesystem/path.hpp>
#include <vector>
#include <functional>

namespace file_monitor
{
class monitor
{
public:
    using path_t = boost::filesystem::path;
    using file_list_t = std::vector<boost::filesystem::path>;
    using change_event_t = std::function<void(path_t const& base_path, file_list_t const& files)>;

    monitor();
    virtual ~monitor();

    virtual void stop() = 0;
    virtual void start(path_t const& base_path) = 0;

    virtual void poll(change_event_t const& consumer) = 0;
private:
};

}



