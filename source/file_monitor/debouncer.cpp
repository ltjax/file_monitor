#include "debouncer.hpp"

using namespace file_monitor;

debouncer::debouncer(std::shared_ptr<monitor> decorated, clock_t::duration countdown_length)
: m_decorated(std::move(decorated))
, m_countdown_length(countdown_length)
{
}

debouncer::~debouncer()
{
}

void debouncer::stop()
{
    m_decorated->stop();
}

void debouncer::start(path_t const& base_path)
{
    m_countdown_started = false;
    m_decorated->start(base_path);
    
}

path_t debouncer::base_path() const
{
    return m_decorated->base_path();
}

void debouncer::poll(change_event_t const& consumer)
{
    m_decorated->poll([this](file_list_t const& files)
    {
        if (!m_countdown_started)
        {
            m_countdown_started = true;
            m_countdown_time = clock_t::now();
        }

        auto& stored(m_files_changed);

        for (auto const& file : files)
        {
            auto found = std::find(stored.begin(), stored.end(), file);
            if (found == stored.end())
                stored.push_back(file);
        }
    });

    if (!m_countdown_started)
        return;

    // calculate the time the timer has been running for
    auto wait_time = clock_t::now() - m_countdown_time;

    if (wait_time > m_countdown_length)
    {
        consumer(m_files_changed);
        m_countdown_started = false;
        m_files_changed.clear();
    }
}
