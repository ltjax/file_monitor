#include "scoped_temp_folder.hpp"
#include <boost/filesystem/fstream.hpp>
#include <catch.hpp>
#include <file_monitor/factory.hpp>
#include <thread>

using namespace Catch::Matchers;

namespace {

void set_file_content(file_monitor::path_t const& path, std::string content)
{
    boost::filesystem::ofstream file(path, std::ios::binary | std::ios::trunc);
    file << content;        
}

}

TEST_CASE("can create monitor")
{
    auto monitor = file_monitor::make_monitor();
    REQUIRE(monitor != nullptr);
}

TEST_CASE("in temporary folder")
{
    scoped_temp_folder folder("file_monitor_test");
    auto monitor = file_monitor::make_monitor();

    SECTION("detects single change")
    {
        auto filename = file_monitor::path_t("test.file");
        auto path = folder.get() / filename;
        set_file_content(path, "before");
        monitor->start(folder);
        set_file_content(path, "after");

        // HACK: See Issue #14
        std::this_thread::sleep_for(std::chrono::seconds(1));

        bool triggered = false;
        auto handler = [&](auto const& base, auto const& files) {
            REQUIRE_THAT(files, VectorContains(filename));
            triggered = true;
        };

        // HACK: See Issue #14 - once that is fixed, only call once and without sleep
        monitor->poll(handler);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        monitor->poll(handler);

        REQUIRE(triggered);
    }
}

