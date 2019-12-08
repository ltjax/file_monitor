#include "monitor.hpp"

#ifdef file_monitor_USE_BOOST
#include <boost/next_prior.hpp>
#include <boost/range/iterator_range.hpp>


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
#else
file_monitor::path_t file_monitor::relative_child_path(file_monitor::path_t const& base,
                                                       file_monitor::path_t const& child)
{
  return child.lexically_relative(base);
}
#endif

std::uint32_t file_monitor::adler32(std::uint8_t const* data, std::size_t size)
{
  std::uint32_t ADLER_PRIME = 65521;
  std::uint32_t a = 1;
  std::uint32_t b = 0;

  for (std::size_t i = 0; i < size; ++i)
  {
    a = (a + data[i]) % ADLER_PRIME;
    b = (b + a) % ADLER_PRIME;
  }

  return (b << 16U) | a;
}
