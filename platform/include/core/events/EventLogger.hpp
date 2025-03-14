#ifndef EVENT_LOGGER_HPP
#define EVENT_LOGGER_HPP

#include "models/Event.hpp"

#include <vector>
#include <queue>
#include <memory>

class EventLogger
{
public:
    EventLogger() = default;
    ~EventLogger() = default;

    void logEvent(std::shared_ptr<Event> event);
    bool getNextEvent(std::shared_ptr<Event> &event);
    std::vector<std::shared_ptr<Event>> getEventLog() const;

private:
    std::queue<std::shared_ptr<Event>> eventQueue;
    std::vector<std::shared_ptr<Event>> eventLog;
};

#endif