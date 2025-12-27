#include "app.h"
#include "system_event.h"
#include <zephyr/logging/log.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/device.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

static struct g_app {
	int count;
} g_app;

static const struct device *wdt_dev;
static int wdt_channel_id;

void watchdog_init(void) {
	wdt_dev = DEVICE_DT_GET(DT_ALIAS(watchdog0));
	if (!device_is_ready(wdt_dev)) {
		LOG_ERR("Error in boot  watchdog");
	}

	struct wdt_timeout_cfg cfg = {
		.window = {
			.min = 0,
			.max = 2000,
		},
		.callback = NULL,
		.flags = WDT_FLAG_RESET_SOC,
	};

	wdt_channel_id = wdt_install_timeout(wdt_dev, &cfg);
	if (wdt_channel_id < 0) {
		LOG_ERR("Error in create watchdog channel");
	}

	if (wdt_setup(wdt_dev, 0) < 0) {
		LOG_ERR("ERROR in setup watchdog");
	}
}

void watchdog_kick(void) {
	wdt_feed(wdt_dev, wdt_channel_id);
}

void app_run(void) {
	g_app.count++;
	LOG_INF("Started: %d", g_app.count);
	system_event_post(EVENT_START);
	enum system_event evt;
	watchdog_init();
	while(1) {
		system_event_get(&evt);
		switch(evt) {
			case EVENT_START:
				LOG_INF("Event start catched");
				break;
			default:
				LOG_ERR("Error in catching event");
				break;
		}
		watchdog_kick();
	}
}
	
