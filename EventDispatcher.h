#ifndef EVENTDISPATCHER_H_INCLUDED
#define EVENTDISPATCHER_H_INCLUDED

#include "Event.h"
#include <functional>
#include <cstdlib>
#include <map>
#include <memory>
#include <vector>
#include <list>
#include <queue>
#include <cstdio>
#include <ctime>
#include <string>
#include <utility>
#include <cassert>

using namespace std;

/**********************************************************
 * IEventObserver
 *
 * Interface of event observer.
 **********************************************************/
class IEventObserver {
public:
	virtual ~IEventObserver() {}
};

/**********************************************************
 * EventDispatcher
 *
 * Responsible for dispatching events
 * to designated listeners.
 **********************************************************/
class EventDispatcher {
public:
	typedef shared_ptr<IEventObserver> EventObserverPtr;
	template <typename T> using ListenerFunction = function<bool(const shared_ptr<T>)>;

//---------------------------------------------------------
// Public interface
//---------------------------------------------------------
													EventDispatcher();
													~EventDispatcher();

	template <typename T> unsigned int 				addAnonymousListener(ListenerFunction<T> listener);
	template <typename U, typename T> void 			addObjectListener(
														shared_ptr<U> observer,
														bool (U::*listener)(const shared_ptr<T>));

	template <typename T, typename ...Args> void 	triggerEvent(Args&& ...args) const;
	template <typename T, typename ...Args> void 	queueEvent(const float delay, Args&& ...args);
	void 											update(float dt);

	void 											removeAnonymousListener(const unsigned int id);
	template <typename T> void 						removeObjectListener(const EventObserverPtr observer);

private:
	class ListenerConcept;
	template <class T> class Listener;

//---------------------------------------------------------
// Internal variables
//---------------------------------------------------------
	unsigned int 									nextListenerId = 0;
	vector<shared_ptr<ListenerConcept>> 			anonymousListenerArray;
	map<EventType, 
		list<shared_ptr<ListenerConcept>>> 			anonymousListenerMap;
	map<EventObserverPtr, 
		list<shared_ptr<ListenerConcept>>> 			objectListenerMap;
	list<pair<EventPtr, float>> 					eventQueue;

//---------------------------------------------------------
// Internal methods
//---------------------------------------------------------
	unsigned int getListenerId();

//---------------------------------------------------------
// Internal classes
//---------------------------------------------------------
	struct ListenerConcept {
	public:
		virtual ~ListenerConcept() {}
		virtual EventType getType() const = 0;
		virtual bool operator()(const EventPtr event) = 0;
	};

	template <typename T>
	struct Listener : public ListenerConcept {
		Listener(const function<bool(const shared_ptr<T>)> func)
		: func(func)
		, type(T::GetType()) {}
		virtual ~Listener() {}

		EventType getType() const {
			return type;
		}

		bool operator()(const EventPtr event) {
			return func(static_pointer_cast<T>(event));
		}

	private:
		EventType type;
		function<bool(const shared_ptr<T>)> func;
	};
};

/**********************************************************
 * Inline implementation of EventDispatcher's
 * templated methods
 **********************************************************/
template <typename T>
inline
unsigned int EventDispatcher::addAnonymousListener(function<bool(const shared_ptr<T>)> listener) {
    auto hey = make_shared<Listener<T>>(listener);
    anonymousListenerMap[T::GetType()].push_back(hey);
    anonymousListenerArray.push_back(hey);
    unsigned int id = getListenerId();
    return id;
}

template <typename ObserverClass, typename T>
inline
void EventDispatcher::addObjectListener(
    shared_ptr<ObserverClass> observer, 
    bool (ObserverClass::*listener)(const shared_ptr<T>)) {

    auto hmm = bind(listener, observer, placeholders::_1);
    auto hey = make_shared<Listener<T>>(hmm);
    anonymousListenerMap[T::GetType()].push_back(hey);
    objectListenerMap[observer].push_back(hey);
}

template <typename T, typename ...Args>
inline
void EventDispatcher::triggerEvent(Args&& ...args) const {
    auto event = std::make_shared<T>(std::forward<Args>(args)...);
    auto &listenerArray = anonymousListenerMap.at(event->getType());
    for (auto listener : listenerArray) {
        (*listener.get())(event);
    }
}

template <typename T, typename ...Args>
inline
void EventDispatcher::queueEvent(const float delay, Args&& ...args) {
    assert(delay > 0);
    const auto event = std::make_shared<T>(std::forward<Args>(args)...);
    eventQueue.push_back(std::pair<EventPtr, const float>(event, delay));
}

template <typename T>
inline
void EventDispatcher::removeObjectListener(const EventObserverPtr observer) {
    for (auto listener : objectListenerMap[observer]) {
        anonymousListenerMap[T::GetType()].remove(listener);
    }
}

#endif