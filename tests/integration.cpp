#include "scoped_temp_folder.hpp"
#include <boost/filesystem/fstream.hpp>
#include <catch.hpp>
#include <file_monitor/factory.hpp>

TEST_CASE("can create monitor")
{
    auto monitor = file_monitor::make_monitor();
    REQUIRE(monitor != nullptr);
}

