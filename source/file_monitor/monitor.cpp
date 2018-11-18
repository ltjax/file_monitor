#include "monitor.hpp"
#include <boost/next_prior.hpp>
#include <boost/range/iterator_range.hpp>

file_monitor::monitor::monitor() = default;
file_monitor::monitor::~monitor() = default;

file_monitor::path_t file_monitor::relative_child_path(file_monitor::path_t const& base,
                                                       file_monitor::path_t const& child)
{
  file_monitor::path_t result;
  auto prefix = std::distance(base.begin(), base.end());
  auto range = boost::make_iterator_range(boost::next(child.begin(), prefix), child.end());

  for (auto const& each : range)
    result /= each;

  return result;
}
