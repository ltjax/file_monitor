#pragma once

#include <CoreServices/CoreServices.h>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <thread>
#include <mutex>
#include <map>
#include "monitor.hpp"

namespace file_monitor
{
class mac_monitor : public monitor
{
public:
    explicit mac_monitor();
    ~mac_monitor() override;

    void stop() override;
    void start(path_t const& path) override;

    void poll(change_event_t const& consumer) override;
private:
    void Run(FSEventStreamRef Stream);

    struct Detail;

    std::string HashFile(boost::filesystem::path const& Filepath);
    void PathChanged(boost::filesystem::path const& Path);
    boost::filesystem::path RelativePath(boost::filesystem::path const& File);
    void HashFilesIn(boost::filesystem::path const& Root);

    boost::filesystem::path mBasePath;
    std::thread mEventThread;
    CFRunLoopRef mLoop = nullptr;
    std::map<boost::filesystem::path, std::string> mFileHash;

    std::mutex mQueueMutex;
    file_list_t mChanged;
    std::atomic_bool mKeepRunning{false};
    CFRunLoopSourceRef mStopSource=nullptr;
};
}