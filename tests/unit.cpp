#include <catch.hpp>
#include <file_monitor/monitor.hpp>

namespace
{
using path_t = file_monitor::path_t;
}

TEST_CASE("can compute relative path")
{
  path_t base =
    "/var/folders/c8/67mrkcvx3bb1bj7qm65xlvxc0000gn/T/file_monitor_testd2d2c-1c64a-4e0a-ba8c";
  path_t file = "/var/folders/c8/67mrkcvx3bb1bj7qm65xlvxc0000gn/T/"
                "file_monitor_testd2d2c-1c64a-4e0a-ba8c/test.file";

  REQUIRE(file_monitor::relative_child_path(base, file) == "test.file");
}
