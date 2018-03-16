#include "win_monitor.hpp"
#include <array>

using namespace file_monitor;

namespace
{
auto const RESULT_BUFFER_SIZE = 4096;
}

win_monitor::win_monitor()
{
    m_result_buffer.resize(RESULT_BUFFER_SIZE);
    m_countdown_length = std::chrono::milliseconds(500);
}

win_monitor::~win_monitor()
{
    if (m_directory_handle != INVALID_HANDLE_VALUE)
        CloseHandle(m_directory_handle);

    if (m_notify_event != nullptr)
        CloseHandle(m_notify_event);
}

void win_monitor::stop()
{
}

void win_monitor::start(path_t const& base_path)
{
    if (m_directory_handle != INVALID_HANDLE_VALUE)
        throw std::runtime_error("FileMonitor already started.");

    m_base_path = base_path;

    // Get a handle for the directory to watch
    m_directory_handle = CreateFile(base_path.string().c_str(), FILE_LIST_DIRECTORY,
                                    FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

    // Create an event for the polling
    m_notify_event = CreateEvent(NULL, FALSE, FALSE, "FileChangedEvent");

    if (m_notify_event == nullptr)
        throw std::runtime_error("FileMonitor failed to create event.");

    m_overlapped_io.hEvent = m_notify_event;

    // TODO: do something with the return value
    listen();
}

void win_monitor::poll(change_event_t const& consumer)
{
    if (m_countdown_started)
    {
        // calculate the time the timer has been running for
        auto Time = clock_t::now() - m_countdown_time;

        if (Time > m_countdown_length)
        {
            consumer(m_base_path, m_files_changed);
            m_countdown_started = false;
            m_files_changed.clear();
        }
    }

    DWORD bytes_written = 0;
    BOOL result = GetOverlappedResult(m_notify_event, &m_overlapped_io, &bytes_written, FALSE);

    // No results yet?
    if (result == FALSE)
    {
        assert(GetLastError() == ERROR_IO_INCOMPLETE);
        return;
    }

    // We got something
    const char* current_entry = m_result_buffer.data();
    while (true)
    {
        // FIXME: is this a well-defined reinterpret?
        auto file_info = reinterpret_cast<FILE_NOTIFY_INFORMATION const*>(current_entry);

        if (file_info->Action == FILE_ACTION_MODIFIED || file_info->Action == FILE_ACTION_RENAMED_NEW_NAME)
        {
            // Note that FileName uses 16-bit chars, while the FileNameLength is in bytes!
            auto character_count = file_info->FileNameLength / 2;
            add_path({ file_info->FileName, file_info->FileName + character_count });
        }

        // If there's another one, go there
        if (file_info->NextEntryOffset == 0)
            break;

        current_entry += file_info->NextEntryOffset;
    }

    // TODO: do something with the bool result
    listen();
}

bool file_monitor::win_monitor::listen()
{
    DWORD unused = 0;

    BOOL result = ReadDirectoryChangesW(m_directory_handle, m_result_buffer.data(), m_result_buffer.size(), TRUE,
                                        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, &unused,
                                        &m_overlapped_io, NULL);

    if (result == 0)
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

    return (result != 0);
}

void win_monitor::add_path(path_t new_filename)
{
    auto& list(m_files_changed);

    if (std::find(list.begin(), list.end(), new_filename) != list.end())
        return;

    list.push_back(std::move(new_filename));

    if (!m_countdown_started)
    {
        m_countdown_started = true;
        m_countdown_time = clock_t::now();
    }
}
