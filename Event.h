#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "HashedString.h"
#include <memory>
#include <ctime>
#include <string>
#include <sstream>

using namespace std;
using namespace std::chrono;

class IEvent;

typedef unsigned long long EventType;
typedef shared_ptr<IEvent> EventPtr;

/**********************************************************
 * IEvent
 *
 * Interface of event.
 **********************************************************/
class IEvent {
public:
    virtual ~IEvent() {}
    virtual time_t getTimestamp() const = 0;
    virtual EventType getType() const = 0;
};

/**********************************************************
 * BaseEvent
 *
 * Abstract event class which is must be subclassed
 * to define an event class.
 **********************************************************/
template <EventType T>
class BaseEvent : public IEvent {
public:

//---------------------------------------------------------
// Public variables
//---------------------------------------------------------
    static constexpr const EventType GetType() { 
        return T; 
    }

    const EventType     type = T;
    const time_t        timestamp;
    const string        typeString;

//---------------------------------------------------------
// Public interface
//---------------------------------------------------------
    BaseEvent(
    	const string &typeString,
    	const time_t timestamp
    	)
    : typeString(typeString)
    , timestamp(timestamp) {}
    virtual ~BaseEvent() {}

    time_t getTimestamp() const {
        return timestamp;
    }

    string getTimestampString() const {
    	auto timeinfo = localtime(&timestamp);
    	// Remove '\n' from the end of the string
		string temp = asctime(timeinfo);
		return temp.substr(0, temp.size() - 1);
    }

    EventType getType() const { 
        return type;
    }
};

/**********************************************************
 * Macros for simplifying event definition
 **********************************************************/
#define EVENT_DEF(__s__) struct __s__ : public BaseEvent<hash_ct(#__s__)>
#define EVENT_DEF_CTOR(__s__, ...) __s__(__VA_ARGS__): BaseEvent(#__s__, time(NULL))
#define EVENT_LISTENER(s, u)	[](const std::shared_ptr<s> u) -> bool

#endif