#include <cstddef>

#define MAX_EVENTS 128
#define MAX_BOUND_CALLBACKS 128


namespace EventSystem {

	typedef enum {

		EVENT_SHUTDOWN,
		EVENT_LAST

	} Event;

	void (*callbacks[MAX_EVENTS][MAX_BOUND_CALLBACKS])(void*) = {0};

	void bind(Event e, void(*callback)(void*)){
		for (int i = 0; i < MAX_BOUND_CALLBACKS; ++i) {
			if( callbacks[e][i] == NULL ) {
				callbacks[e][i] = callback;
				return;
			}
		}
	}

	void unbind(Event e, void(*callback)(void*)){
		for (int i = 0; i < MAX_BOUND_CALLBACKS; ++i) {
			if( callbacks[e][i] == callback ) {
				callbacks[e][i] = NULL;
			}
		}
	}

	void fire(Event e, void*args){
		for (int i = 0; i < MAX_BOUND_CALLBACKS; ++i) {
			if( callbacks[e][i] != NULL ) callbacks[e][i](args);
		}
	}



}