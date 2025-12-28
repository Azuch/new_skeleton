#include "sensor_fsm.h"
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(sensor_fsm, LOG_LEVEL_INF);

//Define enum event for sensor

enum sensor_event {
	SENSOR_EVENT_NONE = 0,
	SENSOR_EVENT_INIT_DONE,
	SENSOR_EVENT_READ_DONE,
	SENSOR_EVENT_ERROR,
};


//Define struct stage for sensor

struct sensor_state {
	SENSOR_STATE_INIT,
	SENSOR_STATE_READ,
	SENSOR_STATE_READY,
	SENSOR_STATE_ERROR,
	SENSOR_STATE_RECOVERY,
};


//Define context for sensor

static struct {
	sensor_state state;
	uint8_t retry_count;
} sensor_ctx;

//Create sensor_data_q

K_MSGQ_DEFINE(sensor_data_q, sizeof(struct sensor_sample), 10, 4);

//Define function sensor_data_available()

void sensor_data_available(void) {
	return k_msgq_num_used_get(&sensor_data_q) > 0;
}

//Define function sensor_data_get()

void sensor_data_get(struct sensor_sample *out) {
	return k_msgq_get(&sensor_data_q, out, K_NO_WAIT) == 0;
}

//Define function sensor_hw_init()
//Let simulate the hardware/sensor init
bool sensor_hw_init(void) {
	return true;	
}

//Define function sensor_hw_read()

static bool sensor_hw_read(struct sensor_sample *out) {
	static int counter;
	counter++;

	if (counter % 5 == 0) {
		return false;
	}

	out->value = counter * 10;
	return true;
}

//Define function sensor_fsm_init()

void sensor_fsm_init(void) {
	sensor_ctx.state = SENSOR_STATE_INIT;
	sensor_ctx.retry_count = 0;
	LOG_INF("Sensor fsm initialized");
}

//Define function sensor_fsm_step()

void sensor_fsm_step(void) {
	struct sensor_sample sample;

	switch (sensor_ctx.state) {
		case SENSOR_STATE_INIT:
			if(!sensor_hw_init()) {
				sensor_ctx.state = SENSOR_STATE_ERROR;
				break;
			}
			sensor_ctx.state = SENSOR_STATE_READ;
			break;
		case SENSOR_STATE_READ:
			if (!sensor_hw_read(&sample)) {
				sensor_ctx.state = SENSOR_STATE_ERROR;
				break;
			}
			sensor_ctx.state = SENSOR_STATE_IDLE;
			break;
		case SENSOR_STATE_IDLE:
			k_msleep(1500);
			sensor_ctx.state = SENSOR_STATE_READ;
			break;
		case SENSOR_STATE_ERROR:
			sensor_ctx.retry_count++;
			if (sensor_ctx.retry_count <= SENSOR_MAX_RETRY) {
				sensor_ctx.state = SENSOR_STATE_IDLE;
			} else {
				sensor_ctx.state = SENSOR_STATE_RECOVERY;
			}
			break;
		case SENSOR_STATE_RECOVERY:
			sensor_ctx.state = SENSOR_STATE_INIT;
			break;
		}
}

