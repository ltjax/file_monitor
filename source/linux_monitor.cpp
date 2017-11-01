#include "linux_monitor.hpp"
#include <sys/poll.h>

using namespace file_monitor;

linux_monitor::linux_monitor()
{
  m_inotify_instance = inotify_init1(IN_NONBLOCK);
  //TODO: decide on whether to throw here if -1 is returned.
  //      would like a valid m_inotify_instance to be an invariant
}

linux_monitor::~linux_monitor()
{
  assert(m_inotify_instance != -1);
  close(m_inotify_instance);
}

void linux_monitor::stop()
{
  assert(m_inotify_instance    != -1);
  assert(m_base_watch.watch_id != -1);
  inotify_rm_watch(m_inotify_instance, m_base_watch.watch_id);
  m_base_watch.watch_id = -1;
}

void linux_monitor::start(path_t const& base_path)
{
  assert(m_inotify_instance != -1);
  m_base_watch.base_path = base_path;
  m_base_watch.watch_id = inotify_add_watch(m_inotify_instance, base_path.c_str(), IN_ALL_EVENTS);
  assert(m_base_watch.watch_id);
}

void linux_monitor::poll(monitor::change_event_t const& consumer)
{
  pollfd poll_descriptor = { m_inotify_instance, POLLIN };

  // we only poll for changes on
  ::poll(&poll_descriptor, 1, 0);

  // assert(!IN_IGNORED): Need to make sure we're not watching deleted
  // directories here
}


