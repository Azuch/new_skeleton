#include "system_event.h"
#include <zephyr/kernel.h>

K_MSGQ_DEFINE(system_event_q, sizeof(enum system_event), 8, 4);

void system_event_post(enum system_event evt) {
	k_msgq_put(&system_event_q, &evt, K_NO_WAIT);
}

int system_event_get(enum system_event *evt) {
	return k_msgq_get(&system_event_q, evt, K_FOREVER);
}
