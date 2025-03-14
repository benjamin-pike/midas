#include "events/EventLogger.hpp"

void EventLogger::logEvent(std::shared_ptr<Event> event)
{
    eventQueue.push(event);
    eventLog.push_back(event);
}

bool EventLogger::getNextEvent(std::shared_ptr<Event> &event)
{
    if (eventQueue.empty())
        return false;
    event = std::move(eventQueue.front());
    eventQueue.pop();
    return true;
}

std::vector<std::shared_ptr<Event>> EventLogger::getEventLog() const
{
    return eventLog;
}