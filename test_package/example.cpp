#include <iostream>
#include <file_monitor/factory.hpp>

int main(int argc, char** argv)
{
  boost::filesystem::path root = ".";
  auto monitor = file_monitor::make_monitor(root);
}
