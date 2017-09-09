#include <cstddef>

#define MAX_EVENTS 128
#define MAX_BOUND_CALLBACKS 128


namespace EventSystem {

	typedef enum {
		EVENT_SHUTDOWN,
		EVENT_LAST
	} Event;

	void bind(Event e, void(*callback)(void*));

	void unbind(Event e, void(*callback)(void*));

	void fire(Event e, void*args);
}