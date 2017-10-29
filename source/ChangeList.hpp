#pragma once

#include <boost/filesystem/path.hpp>
#include <vector>
#include <functional>

class CChangeList
{
public:
    using Path = boost::filesystem::path;
    using FileList = std::vector<boost::filesystem::path>;
    using ChangeEvent = std::function<void(Path const&, FileList const&)>;

    CChangeList();
    virtual ~CChangeList();

    virtual void Stop() = 0;
    virtual void Start(Path const& Path) = 0;

    virtual void Poll(ChangeEvent const& Consumer) = 0;
private:
};



