#pragma once

#include <boost/filesystem/path.hpp>
#include <string>

class scoped_temp_folder
{
public:
    explicit scoped_temp_folder(std::string const& prefix);
    scoped_temp_folder(scoped_temp_folder const&) = delete;
    ~scoped_temp_folder();
    scoped_temp_folder& operator=(scoped_temp_folder const&) = delete;

    operator boost::filesystem::path const&() const
    {
        return get();
    }

    boost::filesystem::path const& get() const
    {
        return m_path;
    }

private:
    boost::filesystem::path m_path;
};
