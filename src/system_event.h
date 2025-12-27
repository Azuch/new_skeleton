#pragma once

enum system_event {
	EVENT_OK = 0,
	EVENT_START,
};

void system_event_post(enum system_event);
int system_event_get(enum system_event *evt);
