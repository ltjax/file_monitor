#include <CoreServices/CoreServices.h>
#include "MacChangeList.hpp"

struct CMacChangeList::Detail
{

    static void ChangeEvent(ConstFSEventStreamRef streamRef,
                            void* clientCallBackInfo,
                            size_t numEvents,
                            void* eventPaths,
                            const FSEventStreamEventFlags eventFlags[],
                            const FSEventStreamEventId eventIds[])
    {
        auto that = static_cast<CMacChangeList*>(clientCallBackInfo);
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

void CMacChangeList::Start(boost::filesystem::path const& Path)
{
    mKeepRunning = true;
    GetLogger()->info("Starting file monitor on '{0}'", Path.string());
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

void CMacChangeList::Run(FSEventStreamRef Stream)
{
    this->mLoop = CFRunLoopGetCurrent();
    GetLogger()->info("Hashing all files in {0}", mBasePath.string());
    this->HashFilesIn(mBasePath);
    GetLogger()->info("Done hashing files..");
    FSEventStreamScheduleWithRunLoop(Stream, mLoop, kCFRunLoopDefaultMode);
    FSEventStreamStart(Stream);
    //if (!mKeepRunning)
    //    return;
    CFRunLoopAddSource(mLoop, mStopSource, kCFRunLoopDefaultMode);

    GetLogger()->info("Starting run loop!");
    CFRunLoopRun();
    GetLogger()->info("Ending run loop!");
}


std::string CMacChangeList::HashFile(boost::filesystem::path const& Filepath)
{
    boost::iostreams::mapped_file_source File(Filepath.string());

    Keccak Hasher;
    return Hasher(File.data(), File.size());
}

void CMacChangeList::HashFilesIn(boost::filesystem::path const& Root)
{
    using Iterator = boost::filesystem::recursive_directory_iterator;
    auto Log = GetLogger();

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
            Log->error("Unable to hash file '{0}': {1}", Entry.path().string(), Error.what());
        }
    }
}

boost::filesystem::path CMacChangeList::RelativePath(boost::filesystem::path const& File)
{
    boost::filesystem::path Result;
    auto Prefix = std::distance(mBasePath.begin(), mBasePath.end());

    for (auto Each : boost::make_iterator_range(boost::next(File.begin(), Prefix), File.end()))
        Result /= Each;

    return Result;
}

void CMacChangeList::PathChanged(boost::filesystem::path const& Path)
{
    // TODO: we can use the non-recurive iterator if the event flags say the change wasnt in subdirs
    using Iterator = boost::filesystem::recursive_directory_iterator;
    auto Log = GetLogger();

    for (auto& Entry : boost::make_iterator_range(Iterator(Path), {}))
    {
        if (!is_regular_file(Entry.status()))
            continue;

        auto Path = RelativePath(Entry.path());

        auto Hash = HashFile(Entry.path());
        auto& OldHash = mFileHash[Path];
        if (Hash != OldHash)
        {
            Log->info("File change detected for '{0}'", Path.string());
            mFileHash[Path] = Hash;
            std::lock_guard<std::mutex> Guard(mQueueMutex);
            mChanged.push_back(Path);
        }
    }
}

std::shared_ptr<spdlog::logger> CMacChangeList::GetLogger() const
{
    return mLogger;
}

void CMacChangeList::Stop()
{
    GetLogger()->info("Sending stop signal!");
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

CMacChangeList::~CMacChangeList()
{
    Stop();
}

CMacChangeList::CMacChangeList(std::shared_ptr<spdlog::logger> Logger)
    : mLogger(std::move(Logger))
{

}

void CMacChangeList::Poll(ChangeEvent const& Consumer)
{
    std::lock_guard<std::mutex> Lock(mQueueMutex);

    if (!mChanged.empty())
    {
        Consumer(mBasePath, mChanged);
        mChanged.clear();
    }
}
