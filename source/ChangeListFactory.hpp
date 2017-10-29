#pragma once


#include <spdlog/logger.h>
#include "ChangeList.hpp"

std::shared_ptr<CChangeList> MakeChangeList(std::shared_ptr<spdlog::logger> Logger);


