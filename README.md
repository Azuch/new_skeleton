Phase 3: Add watchdog
    - the default watchdog in zephyr has alias as watchdog0;
so we will use DEVICE_DT_GET(DT_ALIAS(watchdog0));
    - the watchdog will need wdt_timeout_cfg cfg, which wdt_install_timeout(wdt_dev, &cfg) will use
    - Next is wdt_setup(wdt_dev, 0);
    - next is wdt_feed(wdt_dev, wdt_channel_id);

