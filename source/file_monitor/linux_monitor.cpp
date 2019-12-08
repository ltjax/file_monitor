#include "linux_monitor.hpp"
#include <sys/poll.h>
#include <unistd.h>
#include <cassert>

#ifdef file_monitor_USE_BOOST
#include <boost/filesystem.hpp>
namespace filesystem = boost::filesystem;
#else
namespace filesystem = std::filesystem;
#endif

using namespace file_monitor;

namespace
{
template <typename T> inline void insert_unique(std::vector<T>& container, T const& value)
{
  auto found = std::find(container.begin(), container.end(), value);
  if (found == container.end())
    container.push_back(value);
}

inline path_t join(path_t lhs, path_t const& rhs)
{
#ifdef file_monitor_USE_BOOST
  return absolute(lhs, rhs);
#else
  return (lhs /= rhs);
#endif
}

} // namespace

linux_monitor::linux_monitor(path_t const& base_path)
{
  m_handle = inotify_init1(IN_NONBLOCK);
  if (m_handle == -1)
    throw std::runtime_error("Could not aquire an inotify instance!");

  // for now, we have to only monitor directories and their contents
  // since at least one other implementation has no file-level monitoring
  // support.
  if (!is_directory(base_path))
    throw std::runtime_error("Base path does not exist or is not a directory!");

  m_base_path = canonical(base_path);
  create_watches(m_base_path);
  if (m_watches.empty())
    throw std::runtime_error("Unable to create watches on " + base_path.string());

  m_base_watch_id = m_watches.front().id;
}

linux_monitor::~linux_monitor()
{
  assert(m_handle != -1);

  for (auto const& watch : m_watches)
    inotify_rm_watch(m_handle, watch.id);

  close(m_handle);
}

void linux_monitor::create_watches(path_t const& root)
{
  using directory_iterator = filesystem::directory_iterator;

  auto const id = inotify_add_watch(m_handle, root.c_str(), IN_ALL_EVENTS);
  if (id == -1)
    return;

  m_watches.emplace_back(watch_t{ id, root });

  for (auto i = directory_iterator(root); i != directory_iterator(); ++i)
  {
    if (!is_directory(*i))
      continue;

    create_watches(*i);
  }
}

void linux_monitor::poll(monitor::change_event_t const& consumer)
{
  pollfd poll_descriptor = { m_handle, POLLIN };
  ::poll(&poll_descriptor, 1, 0);

  // check whether there are actual events to process
  if (!(poll_descriptor.revents & POLLIN))
    return;

  process_events(consumer);
}

void linux_monitor::process_event(std::vector<path_t>& changes, inotify_event const* event)
{
  // different events constitute a file change. some tools
  // create temporary files and then move them to the actual
  // file being watched others simply do a normal open/modify/
  // close cycle. file modifications are basically all events
  // that either move TO the file or close the file after writing.

  if (event->len == 0)
  {
    return;
  }

  auto const& watch = find_watch(event->wd);
  if (event->mask & (IN_DELETE_SELF | IN_MOVE_SELF) && watch.id == m_base_watch_id)
  {
    return;
  }

  auto const changed_path = watch.base_path / event->name;

  if (event->mask & IN_CLOSE_WRITE || event->mask & IN_MOVED_TO)
  {
    insert_unique(changes, relative(changed_path, m_base_path));
  }

  auto const absolute_path = join(changed_path, m_base_path);
  if (event->mask & IN_CREATE && filesystem::is_directory(absolute_path))
  {
    create_watches(absolute_path);
  }
  else if (event->mask & (IN_DELETE | IN_MOVED_FROM))
  {
    auto watch =
      std::find_if(m_watches.cbegin(), m_watches.cend(), [absolute_path, this](auto const& watch) {
        auto watch_path_abs = join(watch.base_path, this->m_base_path);
        return watch_path_abs == absolute_path.lexically_normal();
      });

    if (watch != m_watches.end())
    {
      inotify_rm_watch(m_handle, watch->id);
      m_watches.erase(watch);
    }
  }
}

void linux_monitor::process_events(monitor::change_event_t const& consumer)
{
  std::array<char, 4096> events;
  std::vector<path_t> changed_files;
  read_changes(changed_files, events);
  submit_changes(consumer, changed_files);
}

void linux_monitor::read_changes(std::vector<path_t>& changed_files, std::array<char, 4096>& events)
{
  while (true)
  {
    auto bytes_read = read(m_handle, events.data(), events.size());
    if (bytes_read <= 0)
    {
      break;
    }
    for (auto head = events.data(); head < events.data() + bytes_read;)
    {
      auto event = reinterpret_cast<inotify_event const*>(head);
      auto total_length = sizeof(inotify_event) + event->len;
      process_event(changed_files, event);
      head += total_length;
    }
  }
}

void linux_monitor::submit_changes(monitor::change_event_t const& consumer,
                                   std::vector<path_t> const& changed_files) const
{
  if (changed_files.empty())
    return;

  consumer(changed_files);
}

linux_monitor::watch_t const& linux_monitor::find_watch(int id) const
{
  auto watch = std::find_if(
    m_watches.cbegin(), m_watches.cend(), [id](auto const& watch) { return watch.id == id; });
  assert(watch != m_watches.cend());
  return *watch;
}

path_t linux_monitor::base_path() const
{
  return m_base_path;
}
