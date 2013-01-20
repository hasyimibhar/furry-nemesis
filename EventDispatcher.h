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
#include <cassert>
#include <algorithm>
#include <utility>

class IEventObserver {
public:
	virtual ~IEventObserver() {}
};

typedef std::shared_ptr<IEventObserver> EventObserverPtr;

class EventDispatcher;

typedef unsigned int ListenerId;
typedef std::shared_ptr<EventDispatcher> EventDispatcherPtr;

class EventDispatcher {

	class ListenerConcept;
	template <class T> class Listener;

public:
	EventDispatcher() {}
	~EventDispatcher() {}

	template <typename T>
	ListenerId addAnonymousListener(std::function<bool(const std::shared_ptr<T>)> listener) {
		auto hey = std::make_shared<Listener<T>>(listener);
		anonymousListenerMap[T::GetType()].push_back(hey);
		anonymousListenerArray.push_back(hey);
		ListenerId id = getListenerId();
		return id;
	}

	template <typename ObserverClass, typename T>
	void addObjectListener(
		std::shared_ptr<ObserverClass> observer,
		bool (ObserverClass::*listener)(const std::shared_ptr<T>)) {

		auto hmm = std::bind(listener, observer, std::placeholders::_1);
		auto hey = std::make_shared<Listener<T>>(hmm);
		anonymousListenerMap[T::GetType()].push_back(hey);
		objectListenerMap[observer].push_back(hey);
	}

	template <typename T, typename ...Args>
	void triggerEvent(Args&& ...args) const {
		auto event = std::make_shared<T>(std::forward<Args>(args)...);
		auto &listenerArray = anonymousListenerMap.at(event->getType());
		for (auto listener : listenerArray) {
			(*listener.get())(event);
		}
	}

	template <typename T, typename ...Args>
	void queueEvent(const float delay, Args&& ...args) {
		assert(delay > 0);
		const auto event = std::make_shared<T>(std::forward<Args>(args)...);
		eventQueue.push_back(std::pair<EventPtr, const float>(event, delay));
	}

	void update(float dt) {
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

	void removeListener(const ListenerId id) {
		assert(id < nextListenerId);
		auto listener = anonymousListenerArray[id];
		auto &listenerArray = anonymousListenerMap.at(listener->getType());
		listenerArray.remove(listener);
	}

	template <typename T>
	void removeObjectListener(const EventObserverPtr observer) {
		for (auto listener : objectListenerMap[observer]) {
			anonymousListenerMap[T::GetType()].remove(listener);
		}
	}

	ListenerId getListenerId() {
		ListenerId id = nextListenerId++;
		assert(id < nextListenerId); // Check for wrap around
		return id;
	}

private:
	ListenerId nextListenerId = 0;
	std::vector<std::shared_ptr<ListenerConcept>> anonymousListenerArray;
	std::map<EventType, std::list<std::shared_ptr<ListenerConcept>>> anonymousListenerMap;
	
	std::map<EventObserverPtr, std::list<std::shared_ptr<ListenerConcept>>> objectListenerMap;
	
	std::list<std::pair<EventPtr, float>> eventQueue;

	////////////////////////////////
	// Type erasure awesomeness below

	struct ListenerConcept {
	public:
		virtual ~ListenerConcept() {}
		virtual EventType getType() const = 0;
		virtual bool operator()(const EventPtr event) = 0;
	};

	template <typename T>
	struct Listener : public ListenerConcept {
		Listener(const std::function<bool(const std::shared_ptr<T>)> func) : func(func), type(T::GetType()) {}
		virtual ~Listener() {}

		EventType getType() const {
			return type;
		}

		bool operator()(const EventPtr event) {
			return func(std::static_pointer_cast<T>(event));
		}

	private:
		EventType type;
		std::function<bool(const std::shared_ptr<T>)> func;
	};
};

#endif