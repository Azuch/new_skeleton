#include "http_fsm.h"
#include "wifi_fsm.h"
#include "sensor_fsm.h"

#include <zephyr/kernel.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>

#include <string.h>
#include <stdint.h>

#define HTTP_BACKOFF_MS 2000

LOG_MODULE_REGISTER(HTTP_FSM, LOG_LEVEL_INF);

enum http_state {
	HTTP_STATE_INIT,
	HTTP_STATE_IDLE,
	HTTP_STATE_SEND,
	HTTP_STATE_BACKOFF,
}

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

void set_state(enum http_state next) {
        if (http_ctx.state != next) {
                LOG_INF("State %s -> %s", state_to_str(http_ctx.state), state_to_str(next));
                http_ctx.state = next;
        }
}

//Define a http_client_send helper
bool http_client_send(const char* data) {
	int sock;
	struct addr_in addr;

	sock = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		LOG_ERR("In creating socket");
		return false;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(8899);
	zsock_inet_pton(AF_INET, "192.168.0.189", &addr.sin_family);

	if (zsock_connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		LOG_ERR("In connecting socket");
		zsock_close(sock);
		return false;
	}

	zsock_send(sock, data, strlen(data), 0);
	zsock_close(sock);
	return true;
}

void http_fsm_init(void) {
	LOG_INF("HTTP FSM initialized.");
	http_fsm.state = HTTP_STATE_INIT;
}

void http_fsm_init(void) {

	struct sensor_sample *sample;
	char data[64];

	switch (http_fsm.state) {
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
			if (sensor_data_get(sample)) {

				snprintk(data, sizeof(data), "{\"value\": %d}", sample->value);

				if (http_client_send(data)) {
					set_state(HTTP_STATE_IDLE);
					break;
				}
			} else {
				http_ctx.backoff_until = k_uptime_get() + HTTP_BACKOFF_MS;
				set_state(HTTP_STATE_BACKOFF);
				break;
			}
		case HTTP_STATE_BACKOFF:
			if(k_uptime_get() >= http_ctx.backoff_until) {
				set_state(HTTP_STATE_INIT);
				break;
			}
	}
}
