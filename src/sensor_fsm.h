#pragma once
#include <stdint.h>
#include <stdbool.h>

struct sensor_sample {
	int32_t value;
};

void sensor_fsm_init(void);

void sensor_fsm_step(void);

bool sensor_data_available(void);
bool sensor_data_get(struct sensor_sample *out);
