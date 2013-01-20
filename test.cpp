#include "EventDispatcher.h"
#include <unistd.h>

EVENT_DEF(TestEvent) {
	EVENT_DEF_CTOR(TestEvent, 
		const std::string &message,
		const float number)
	, message(message)
	, number(number) {

	}

	const std::string message;
	const float number;
};

class MyObserver: public IEventObserver {
public:
	bool onTestEvent(const std::shared_ptr<TestEvent> event) {
		printf("[MyObserver] Message received!\n");
		printf("- Type (hashed): %llu\n", 	event->type);
		printf("- Type string: %s\n", 		event->typeString.c_str());
		printf("- Timestamp: %s\n",			event->getTimestampString().c_str());
		printf("- Message: %s\n", 			event->message.c_str());
		printf("- Number: %.2f\n",			event->number);
		return true;
	}
};

int main(int argc, char **argv) {

	auto dispatcher = std::make_shared<EventDispatcher>();

	dispatcher->addAnonymousListener<TestEvent>(
		EVENT_LISTENER(TestEvent, event) {
			printf("[Anonymous] Message received!\n");
			printf("- Type (hashed): %llu\n", 	event->type);
			printf("- Type string: %s\n", 		event->typeString.c_str());
			printf("- Timestamp: %s\n",			event->getTimestampString().c_str());
			printf("- Message: %s\n", 			event->message.c_str());
			printf("- Number: %.2f\n",			event->number);
			return true;
		});

	auto myObserver = std::make_shared<MyObserver>();
	dispatcher->addObjectListener(myObserver, &MyObserver::onTestEvent);

	dispatcher->triggerEvent<TestEvent>("This message is dispatched instantaneously!", 69.0f);
	dispatcher->queueEvent<TestEvent>(5.0f, "This message is delayed...", 12345.0f);
	sleep(1);
	dispatcher->update(2.0f);
	sleep(1);
	dispatcher->update(2.0f);
	sleep(1);
	dispatcher->update(2.0f);

	return EXIT_SUCCESS;
}
