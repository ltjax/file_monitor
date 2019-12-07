#include "scoped_temp_folder.hpp"

#ifdef file_monitor_USE_BOOST
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;
using boost::filesystem::unique_path;
#else
#include <filesystem>
#include <random>
namespace fs = std::filesystem;

inline fs::path unique_path(std::string pattern)
{
  auto constexpr digits = "0123456789abcdef";
  std::uniform_int_distribution<std::size_t> digit_distribution(0, 15);
  std::random_device device;
  for (auto& each : pattern)
  {
    if (each != '%')
      continue;

    each = digits[digit_distribution(device)];
  }
  return { pattern };
}
#endif

scoped_temp_folder::scoped_temp_folder(std::string const& prefix)
{
  auto parent = fs::temp_directory_path();
  auto folder = unique_path(prefix + "%%%%%-%%%%%-%%%%-%%%%");
  auto path = parent / folder;
  fs::create_directory(path);
  m_path = path;
}

scoped_temp_folder::~scoped_temp_folder()
{
  fs::remove_all(m_path);
}
