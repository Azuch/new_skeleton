#pragma once

#include <stdbool.h>

bool wifi_is_connected(void);
void wifi_fsm_init(void);
void wifi_fsm_step(void);
