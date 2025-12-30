#include "http_fsm.h"
#include "wifi_fsm.h"
#include "sensor_fsm.h"
#include "http_transport.h"

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define HTTP_BACKOFF_MS 2000

LOG_MODULE_REGISTER(HTTP_FSM, LOG_LEVEL_INF);

enum http_state {
	HTTP_STATE_INIT,
	HTTP_STATE_IDLE,
	HTTP_STATE_SEND,
	HTTP_STATE_BACKOFF,
};

static struct {
	enum http_state state;
	uint64_t backoff_until;
} http_ctx;


//Define a state_to_str helper

static char* state_to_str(enum http_state s) {
        switch(s) {
		case HTTP_STATE_INIT:		return "INIT";
                case HTTP_STATE_IDLE:         return "IDLE";
                case HTTP_STATE_SEND:         return "SEND";
                case HTTP_STATE_BACKOFF:     return "BACKOFF";
                default:                        return "UNKNOWN";

        }
}
//Define a set_state helper

static void set_state(enum http_state next) {
        if (http_ctx.state != next) {
                LOG_INF("State %s -> %s", state_to_str(http_ctx.state), state_to_str(next));
                http_ctx.state = next;
        }
}

void http_fsm_init(void) {
	LOG_INF("HTTP FSM initialized.");
	http_ctx.state = HTTP_STATE_INIT;
}

void http_fsm_step(void) {

	struct sensor_sample sample;
	char data[64];

	switch (http_ctx.state) {
		case HTTP_STATE_INIT:
			if(wifi_is_connected()) {
				set_state(HTTP_STATE_IDLE);
				break;
			} else {
				break;
			}
		case HTTP_STATE_IDLE:
			if(sensor_data_available()) {
				set_state(HTTP_STATE_SEND);
				break;
			} else {
				break;
			}
		case HTTP_STATE_SEND:
			if (sensor_data_get(&sample)) {

				snprintf(data, sizeof(data), "{\"value\": %d}", sample.value);

				if (http_transport_send(data)) {
					set_state(HTTP_STATE_IDLE);
					break;
				}
			}

			http_ctx.backoff_until = k_uptime_get() + HTTP_BACKOFF_MS;
			set_state(HTTP_STATE_BACKOFF);
			break;
		case HTTP_STATE_BACKOFF:
			if(k_uptime_get() >= http_ctx.backoff_until) {
				set_state(HTTP_STATE_INIT);
				break;
			}
	}
}
