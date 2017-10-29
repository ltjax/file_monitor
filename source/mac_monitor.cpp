#include <CoreServices/CoreServices.h>
#include "mac_monitor.hpp"

struct file_monitor::mac_monitor::Detail
{

    static void ChangeEvent(ConstFSEventStreamRef streamRef,
                            void* clientCallBackInfo,
                            size_t numEvents,
                            void* eventPaths,
                            const FSEventStreamEventFlags eventFlags[],
                            const FSEventStreamEventId eventIds[])
    {
        auto that = static_cast<mac_monitor*>(clientCallBackInfo);
        auto paths = reinterpret_cast<char**>(eventPaths);

        for (int i = 0; i < numEvents; i++)
        {
            that->PathChanged(paths[i]);
        }
    }

    static void StopSourceSignalled(void*)
    {
        CFRunLoopStop(CFRunLoopGetCurrent());
    }

};

void file_monitor::mac_monitor::start(boost::filesystem::path const& Path)
{
    mKeepRunning = true;
    this->mBasePath = Path;
    CFStringRef PathAsStringRef = CFStringCreateWithCString(nullptr, Path.string().c_str(), kCFStringEncodingUTF8);
    CFArrayRef PathsToWatch = CFArrayCreate(NULL, (const void**) &PathAsStringRef, 1, NULL);
    CFAbsoluteTime Latency = 1.0; /* Latency in seconds */
    FSEventStreamContext Context{};
    Context.info = this;

    /* Create the stream, passing in a callback */
    auto Stream = FSEventStreamCreate(NULL, &Detail::ChangeEvent, &Context, PathsToWatch,
                                      kFSEventStreamEventIdSinceNow, Latency, kFSEventStreamCreateFlagNone);

    CFRunLoopSourceContext StopSourceContext = {};
    StopSourceContext.perform = &Detail::StopSourceSignalled;
    mStopSource = CFRunLoopSourceCreate(NULL, 0, &StopSourceContext);

    this->mEventThread = std::thread([this, Stream]()
                                     {
                                         Run(Stream);
                                     });
}

void file_monitor::mac_monitor::Run(FSEventStreamRef Stream)
{
    this->mLoop = CFRunLoopGetCurrent();
    this->HashFilesIn(mBasePath);
    FSEventStreamScheduleWithRunLoop(Stream, mLoop, kCFRunLoopDefaultMode);
    FSEventStreamStart(Stream);

    CFRunLoopAddSource(mLoop, mStopSource, kCFRunLoopDefaultMode);
    CFRunLoopRun();
}


/*std::string file_monitor::CMacChangeList::HashFile(boost::filesystem::path const& Filepath)
{
    boost::iostreams::mapped_file_source File(Filepath.string());

    Keccak Hasher;
    return Hasher(File.data(), File.size());
}*/

void file_monitor::mac_monitor::HashFilesIn(boost::filesystem::path const& Root)
{
    using Iterator = boost::filesystem::recursive_directory_iterator;

    for (auto& Entry : boost::make_iterator_range(Iterator(Root), {}))
    {
        if (!mKeepRunning)
            return;

        if (!is_regular_file(Entry.status()))
            continue;

        auto Path = RelativePath(Entry.path());

        try
        {
            auto Hash = HashFile(Entry.path());
            mFileHash[Path] = Hash;
        }
        catch (std::exception const& Error)
        {
          // TODO: log this?
        }
    }
}

boost::filesystem::path file_monitor::mac_monitor::RelativePath(boost::filesystem::path const& File)
{
    boost::filesystem::path Result;
    auto Prefix = std::distance(mBasePath.begin(), mBasePath.end());

    for (auto Each : boost::make_iterator_range(boost::next(File.begin(), Prefix), File.end()))
        Result /= Each;

    return Result;
}

void file_monitor::mac_monitor::PathChanged(boost::filesystem::path const& Path)
{
    // TODO: we can use the non-recurive iterator if the event flags say the change wasnt in subdirs
    using Iterator = boost::filesystem::recursive_directory_iterator;

    for (auto& Entry : boost::make_iterator_range(Iterator(Path), {}))
    {
        if (!is_regular_file(Entry.status()))
            continue;

        auto Path = RelativePath(Entry.path());

        auto Hash = HashFile(Entry.path());
        auto& OldHash = mFileHash[Path];
        if (Hash != OldHash)
        {
            mFileHash[Path] = Hash;
            std::lock_guard<std::mutex> Guard(mQueueMutex);
            mChanged.push_back(Path);
        }
    }
}

void file_monitor::mac_monitor::stop()
{
    mKeepRunning = false;
    if (mEventThread.joinable())
    {
        CFRunLoopSourceSignal(mStopSource);
        CFRunLoopWakeUp(mLoop);

        // NOTE: Race condition if we're past the last keepRunning State and not in the run loop yet
        //CFRunLoopStop(mLoop);
        mEventThread.join();
    }
}

file_monitor::mac_monitor::~mac_monitor()
{
    stop();
}

file_monitor::mac_monitor::mac_monitor()
{

}

void file_monitor::mac_monitor::poll(change_event_t const& consumer)
{
    std::lock_guard<std::mutex> Lock(mQueueMutex);

    if (!mChanged.empty())
    {
        consumer(mBasePath, mChanged);
        mChanged.clear();
    }
}
