Phase 3: Add watchdog
    - the default watchdog in zephyr has alias as watchdog0;
so we will use DEVICE_DT_GET(DT_ALIAS(watchdog0));
    - the watchdog will need wdt_timeout_cfg cfg, which wdt_install_timeout(wdt_dev, &cfg) will use
    - Next is wdt_setup(wdt_dev, 0);
    - next is wdt_feed(wdt_dev, wdt_channel_id);

Phase 6: Add https protocol
    - This phase use https + get + www.google.com + r1.der inside src/. This r1.der is the google root 1's CA. Please change the HTTP GET  to POST; your server name instead of google; and the path too.
    - For this phase; i added to those files: CMakeLists.txt; prj.conf; secrets.txt; tls.conf;
boards/esp32s3-devkitc.conf; http_transport.c/h; app.c; http_fsm.c/h.
    - The most matter is this phase is the setting inside tls.conf. You must learn to set it in menuconfig; then read the build/zephyr/.config
    - The MBedtls need to use the PSA cryto; so enable all. But remember; RSA, especically the 4096 version; may to big for esp32 to handle.
    - The procedure is first set the config using menuconfig (you can bypass this if using my prj, and tls.conf and set your secrets.conf;): west build -b esp32s3_devkitc/esp32s3/procpu -S flash-16M -S psram-8M -p always -t menuconfig -- -DCONF_FILE="prj.conf;tls.conf;secrets.conf"
    - Next is just: west build -b esp32s3_devkitc/esp32s3/procpu
    - I am tweaking the psram now.
