#include "scoped_temp_folder.hpp"
#include <boost/filesystem/operations.hpp>

scoped_temp_folder::scoped_temp_folder(std::string const& prefix)
{
  auto parent = boost::filesystem::temp_directory_path();
  auto folder = boost::filesystem::unique_path(prefix + "%%%%%-%%%%%-%%%%-%%%%");
  auto path = parent / folder;
  boost::filesystem::create_directory(path);
  m_path = path;
}

scoped_temp_folder::~scoped_temp_folder()
{
  boost::filesystem::remove_all(m_path);
}
