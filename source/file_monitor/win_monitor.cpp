#include "win_monitor.hpp"
#include <array>
#include <cassert>

using namespace file_monitor;

namespace
{
auto const RESULT_BUFFER_SIZE = 4096;
}

win_monitor::win_monitor(path_t const& base_path)
{
  m_result_buffer.resize(RESULT_BUFFER_SIZE);
  m_base_path = base_path;

  // Get a handle for the directory to watch
  m_directory_handle = CreateFileW(base_path.wstring().c_str(),
                                   FILE_LIST_DIRECTORY,
                                   FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                   NULL);

  // Create an event for the polling
  m_notify_event = CreateEventA(NULL, FALSE, FALSE, "FileChangedEvent");

  if (m_notify_event == nullptr)
    throw std::runtime_error("FileMonitor failed to create event.");

  m_overlapped_io.hEvent = m_notify_event;

  listen();
}

win_monitor::~win_monitor()
{
  if (m_directory_handle != INVALID_HANDLE_VALUE)
    CloseHandle(m_directory_handle);

  if (m_notify_event != nullptr)
    CloseHandle(m_notify_event);
}

path_t win_monitor::base_path() const
{
  return m_base_path;
}

void win_monitor::poll(change_event_t const& consumer)
{
  DWORD bytes_written = 0;
  BOOL result = GetOverlappedResult(m_notify_event, &m_overlapped_io, &bytes_written, FALSE);

  // No results yet?
  if (result == FALSE)
  {
    assert(GetLastError() == ERROR_IO_INCOMPLETE);
    return;
  }

  // We got something
  m_files_changed.clear();

  const char* current_entry = m_result_buffer.data();
  while (true)
  {
    auto file_info = reinterpret_cast<FILE_NOTIFY_INFORMATION const*>(current_entry);

    if (file_info->Action == FILE_ACTION_MODIFIED ||
        file_info->Action == FILE_ACTION_RENAMED_NEW_NAME)
    {
      // Note that FileName uses 16-bit chars, while the FileNameLength is in bytes!
      auto character_count = file_info->FileNameLength / 2;
      m_files_changed.push_back({ file_info->FileName, file_info->FileName + character_count });
    }

    // If there's another one, go there
    if (file_info->NextEntryOffset == 0)
      break;

    current_entry += file_info->NextEntryOffset;
  }

  consumer(m_files_changed);

  listen();
}

void file_monitor::win_monitor::listen()
{
  DWORD unused = 0;

  BOOL result = ReadDirectoryChangesW(m_directory_handle,
                                      m_result_buffer.data(),
                                      static_cast<DWORD>(m_result_buffer.size()),
                                      TRUE,
                                      FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
                                      &unused,
                                      &m_overlapped_io,
                                      NULL);

  if (result == 0)
  {
    LPVOID buffer;

    if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                        NULL,
                        GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                        (LPSTR)&buffer,
                        0,
                        NULL))
    {
      throw std::runtime_error("Could not format error message for ReadDirectoryChangesW failure");
    }

    auto message = std::string((const char*)buffer);
    LocalFree(buffer);

    throw std::runtime_error("Unable to start directory watch: " + message);
  }
}
