#include "Arduino.h"
#include "esp_log.h"
#include "esp_pm.h"

#include "blue.h"
#include "RGB.h"
#include "power_monitor.h"

#include "config.h"

void setBatteryLevel(TimerHandle_t)
{
    auto level = PowerMon::getBatteryLevel();
    ESP_LOGI("Battery", "Level: %d", level);
    blue::setBatteryLevel(level);
}

extern "C" void app_main()
{
    initArduino();
    pinMode(KEY1_PIN, INPUT_PULLUP);
    pinMode(KEY2_PIN, INPUT_PULLUP);
    pinMode(KEY3_PIN, INPUT_PULLUP);

    esp_pm_config_t pm_config = {
        .max_freq_mhz = 48,
        .min_freq_mhz = 8,
        .light_sleep_enable = true,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    RGB::init();
    blue::init();
    PowerMon::init();

    auto powerMonTimer = xTimerCreate("BatteryLevel", pdMS_TO_TICKS(5000), pdTRUE, nullptr, setBatteryLevel);
    xTimerStart(powerMonTimer, 0);
    while (1)
    {
        /* code */
    }
    
}