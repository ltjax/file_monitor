#include "scoped_temp_folder.hpp"
#include <boost/filesystem/fstream.hpp>
#include <catch.hpp>
#include <file_monitor/factory.hpp>
#include <thread>

using namespace Catch::Matchers;

namespace
{
void set_file_content(file_monitor::path_t const& path, std::string content)
{
  boost::filesystem::ofstream file(path, std::ios::binary | std::ios::trunc);
  file << content;
}
} // namespace

TEST_CASE("in temporary folder")
{
  scoped_temp_folder folder("file_monitor_test");

  SECTION("detects single change")
  {
    auto filename = file_monitor::path_t("test.file");
    auto path = folder.get() / filename;
    set_file_content(path, "before");
    auto monitor = file_monitor::make_platform_monitor(folder);

    std::this_thread::sleep_for(std::chrono::seconds(1));
    set_file_content(path, "after");

    bool triggered = false;
    auto handler = [&](auto const& files) {
      REQUIRE_THAT(files, VectorContains(filename));
      triggered = true;
    };

    // NOTE: The change detection from the filesystem is not instantaneous, so we need to wait a bit
    // This is, of course, super flaky, but I do not know how to make this better.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    monitor->poll(handler);

    REQUIRE(triggered);
  }
}
