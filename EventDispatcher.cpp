#include "EventDispatcher.h"
#include <algorithm>

EventDispatcher::EventDispatcher() {
}

EventDispatcher::~EventDispatcher() {
}

void EventDispatcher::update(float dt) {
    std::list<std::pair<EventPtr, float>> finishedEventList;

    for (auto &eventPair : eventQueue) {
        auto event = eventPair.first;
        auto &remainingTime = eventPair.second;
        remainingTime -= dt;
        if (remainingTime <= 0) {
            auto &listenerArray = anonymousListenerMap.at(event->getType());
            for (auto listener : listenerArray) {
                (*listener.get())(event);
            }
            finishedEventList.push_back(eventPair);
        }
    }

    for (auto &eventPair : finishedEventList) {
        eventQueue.remove(eventPair);
    }
}

void EventDispatcher::removeAnonymousListener(const unsigned int id) {
    assert(id < nextListenerId);
    auto listener = anonymousListenerArray[id];
    auto &listenerArray = anonymousListenerMap.at(listener->getType());
    listenerArray.remove(listener);
}

unsigned int EventDispatcher::getListenerId() {
    unsigned int id = nextListenerId++;
    assert(id < nextListenerId); // Check for wrap around
    return id;
}
