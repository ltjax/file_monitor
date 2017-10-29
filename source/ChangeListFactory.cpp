#include "ChangeListFactory.hpp"

#if defined(__APPLE__)
#include "MacChangeList.hpp"
#endif

std::shared_ptr<CChangeList> MakeChangeList(std::shared_ptr<spdlog::logger> Logger)
{
#if defined(__APPLE__)
    return std::make_shared<CMacChangeList>(std::move(Logger));
#else
    return nullptr;
#endif
}
