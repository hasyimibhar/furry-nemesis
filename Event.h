#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "HashedString.h"
#include <memory>
#include <ctime>
#include <string>
#include <sstream>

using namespace std::chrono;

typedef unsigned long long EventType;

class IEvent;
typedef std::shared_ptr<IEvent> EventPtr;

class IEvent {
public:
    virtual ~IEvent() {}
    virtual std::time_t getTimestamp() const = 0;
    virtual EventType getType() const = 0;
};

template <EventType T>
class BaseEvent : public IEvent {
public:
    BaseEvent(
    	const std::string &typeString,
    	const std::time_t timestamp
    	)
    : typeString(typeString)
    , timestamp(timestamp) {}
    virtual ~BaseEvent() {}

    std::time_t getTimestamp() const {
        return timestamp;
    }

    std::string getTimestampString() const {
    	auto timeinfo = localtime(&timestamp);
    	// Remove '\n' from the end of the string
		std::string temp = asctime(timeinfo);
		return temp.substr(0, temp.size() - 1);
    }

    EventType getType() const { return type; }
    
    static constexpr const EventType GetType() { return T; }
    const EventType type = T;
    const std::time_t timestamp;
    const std::string typeString;
};

// Macro for simplifying Event definition
#define EVENT_DEF(__s__) struct __s__ : public BaseEvent<hash(#__s__)>
#define EVENT_DEF_CTOR(__s__, ...) __s__(__VA_ARGS__): BaseEvent(#__s__, time(NULL))
#define EVENT_LISTENER(s, u)	[](const std::shared_ptr<s> u) -> bool

#endif