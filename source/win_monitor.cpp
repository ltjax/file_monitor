#include "win_monitor.hpp"

using namespace file_monitor;
#include "win_monitor.hpp"

namespace
{
auto const RESULT_BUFFER_SIZE = 4096;
}

win_monitor::win_monitor()
{
    this->mResultBuffer.resize(RESULT_BUFFER_SIZE);
    this->CountdownLength = std::chrono::milliseconds(500);
}

win_monitor::~win_monitor()
{
    if (this->DirectoryHandle != INVALID_HANDLE_VALUE)
        CloseHandle(this->DirectoryHandle);

    if (this->NotifyEvent != nullptr)
        CloseHandle(this->NotifyEvent);
}

void win_monitor::stop()
{
}

void win_monitor::start(path_t const & base_path)
{
    if (DirectoryHandle != INVALID_HANDLE_VALUE)
        throw std::runtime_error("FileMonitor already started.");
    
    this->BasePath = base_path;

    // Get a handle for the directory to watch
    this->DirectoryHandle =
        CreateFile(base_path.string().c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    // Create an event for the polling
    this->NotifyEvent = CreateEvent(NULL, FALSE, FALSE, "FileChangedEvent");

    if (this->NotifyEvent == nullptr)
        throw std::runtime_error("FileMonitor failed to create event.");

    this->OverlappedIO.hEvent = this->NotifyEvent;

    // TODO: do something with the return value
    this->listen();
}

void win_monitor::poll(change_event_t const& consumer)
{
    if (this->CountdownStarted)
    {
        // calculate the time the timer has been running for
        auto Time = clock_t::now() - this->CountdownTime;

        if (Time > this->CountdownLength)
        {
            consumer(this->BasePath, this->FilesChanged);
            this->CountdownStarted = false;
            this->FilesChanged.clear();
        }
    }

    DWORD BytesWritten = 0;
    BOOL Result = GetOverlappedResult(this->NotifyEvent, &this->OverlappedIO, &BytesWritten, FALSE);

    // No results yet?
    if (Result == FALSE)
    {
        assert(GetLastError() == ERROR_IO_INCOMPLETE);
        return;
    }

    // We got something
    const char* CurrentEntry = mResultBuffer.data();
    // TODO: return buffer size literals here
    char Filename[256];
    while (true)
    {
        const FILE_NOTIFY_INFORMATION* FileInfo = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(CurrentEntry);

        if (FileInfo->Action == FILE_ACTION_MODIFIED || FileInfo->Action == FILE_ACTION_RENAMED_NEW_NAME)
        {
            size_t converted_count = 0;
            // Convert to ASCII
            wcstombs_s(&converted_count, Filename, FileInfo->FileNameLength, FileInfo->FileName, 256);
            Filename[std::min<DWORD>(FileInfo->FileNameLength / 2, 255)] = 0;

            add_path(Filename);
        }

        // If there's another one, go there
        if (FileInfo->NextEntryOffset == 0)
            break;

        CurrentEntry += FileInfo->NextEntryOffset;
    }

    // TODO: do something with the bool result
    listen();
}

bool file_monitor::win_monitor::listen()
{
    DWORD Unused = 0;

    BOOL Result = ReadDirectoryChangesW(this->DirectoryHandle, this->mResultBuffer.data(), this->mResultBuffer.size(), TRUE,
        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, &Unused,
        &this->OverlappedIO, NULL);

    if (Result == 0)
    {
        LPVOID buffer;

        // TODO: remove printf
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, NULL))
            printf("ReadDirectoryChangesW failed with '%s'\n", (const char*)buffer);
        else
            printf("Couldn't format error msg for ReadDirectoryChangesW failure.\n");

        LocalFree(buffer);
    }

    return (Result != 0);
}

void win_monitor::add_path(char const * filename)
{
    path_t Path(filename);

    auto& Vector(this->FilesChanged);

    if (std::find(Vector.begin(), Vector.end(), Path) == Vector.end())
    {
        Vector.push_back(Path);

        if (!this->CountdownStarted)
        {
            this->CountdownStarted = true;
            this->CountdownTime = clock_t::now();
        }
    }
}
