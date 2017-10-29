#pragma once

#include <CoreServices/CoreServices.h>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <hash-library/keccak.h>
#include <thread>
#include <mutex>
#include <map>
#include <spdlog/logger.h>
#include "ChangeList.hpp"

class CMacChangeList : public CChangeList
{
public:
    using Path = boost::filesystem::path;
    using FileList = std::vector<boost::filesystem::path>;
    using ChangeEvent = std::function<void(Path const&, FileList const&)>;

    explicit CMacChangeList(std::shared_ptr<spdlog::logger> Logger);
    ~CMacChangeList() override;

    void Stop() override;
    void Start(Path const& Path) override;

    void Poll(ChangeEvent const& Consumer) override;
private:
    void Run(FSEventStreamRef Stream);

    struct Detail;

    std::string HashFile(boost::filesystem::path const& Filepath);
    std::shared_ptr<spdlog::logger> GetLogger() const;
    void PathChanged(boost::filesystem::path const& Path);
    boost::filesystem::path RelativePath(boost::filesystem::path const& File);
    void HashFilesIn(boost::filesystem::path const& Root);


    std::shared_ptr<spdlog::logger> mLogger;
    boost::filesystem::path mBasePath;
    std::thread mEventThread;
    CFRunLoopRef mLoop = nullptr;
    std::map<boost::filesystem::path, std::string> mFileHash;

    std::mutex mQueueMutex;
    FileList mChanged;
    std::atomic_bool mKeepRunning{false};
    CFRunLoopSourceRef mStopSource=nullptr;
};