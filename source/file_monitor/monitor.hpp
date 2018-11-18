#pragma once

#include <boost/filesystem/path.hpp>
#include <functional>
#include <vector>

namespace file_monitor
{
using path_t = boost::filesystem::path;
using file_list_t = std::vector<path_t>;

class monitor
{
public:
  using change_event_t = std::function<void(file_list_t const& files)>;

  monitor() = default;
  virtual ~monitor() = default;

  virtual path_t base_path() const = 0;
  virtual void poll(change_event_t const& consumer) = 0;
private:
};

/** Given a file 'child' somewhere in the folder base or one of its subfolders,
 * returns the relative path from base to child.
 * Undefined if child is not in base.
 */
path_t relative_child_path(path_t const& base, path_t const& child);

/** Return ADLER32 checksum for given data-buffer.
 */

std::uint32_t adler32(std::uint8_t const* data, std::size_t size);
}
