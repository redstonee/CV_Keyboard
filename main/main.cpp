#include "Arduino.h"
#include <Preferences.h>
#include "esp_log.h"
#include "esp_pm.h"

#include "blue.h"
#include "RGB.h"
#include "power_monitor.h"

#include "config.h"

void setBatteryLevel(TimerHandle_t)
{
    if (!blue::isConnected())
        return;

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
        .light_sleep_enable = true};
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    TaskHandle_t rgbWaitTask;
    auto connectTimeoutCB = [&rgbWaitTask](NimBLEAdvertising *adv)
    {
        if (blue::isConnected())
            return;

        vTaskDelete(rgbWaitTask);
        RGB::fuckOff();
        ESP_LOGI("BLE", "Connection timeout");

        ESP_ERROR_CHECK(gpio_wakeup_enable(static_cast<gpio_num_t>(KEY1_PIN), GPIO_INTR_LOW_LEVEL));
        ESP_ERROR_CHECK(gpio_wakeup_enable(static_cast<gpio_num_t>(KEY2_PIN), GPIO_INTR_LOW_LEVEL));
        ESP_ERROR_CHECK(gpio_wakeup_enable(static_cast<gpio_num_t>(KEY3_PIN), GPIO_INTR_LOW_LEVEL));
        ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());

        esp_light_sleep_start();
    };

    RGB::init();
    blue::init(connectTimeoutCB);
    PowerMon::init();

    xTaskCreate(RGB::waitForPairingTask, "RGB waiting", 2048, nullptr, 1, &rgbWaitTask);

    while (!blue::isConnected())
    {
        delay(500);
    }

    auto powerMonTimer = xTimerCreate("BatteryLevel", pdMS_TO_TICKS(5000), pdTRUE, nullptr, setBatteryLevel);
    xTimerStart(powerMonTimer, 0);

    auto handleKey = [](void *param)
    {
        auto pin = (uint8_t)param;
        auto isPressed = !digitalRead(pin);

        switch (pin)
        {
        case KEY1_PIN:
            RGB::fadeIn(0);
            blue::onKeyStatusChange(0, isPressed);
            break;
        case KEY2_PIN:
            RGB::fadeIn(1);
            blue::onKeyStatusChange(1, isPressed);
            break;
        case KEY3_PIN:
            RGB::fadeIn(2);
            blue::onKeyStatusChange(2, isPressed);
            break;
        default:
            break;
        }
    };

    attachInterruptArg(KEY1_PIN, handleKey, (void *)KEY1_PIN, CHANGE);
    attachInterruptArg(KEY2_PIN, handleKey, (void *)KEY2_PIN, CHANGE);
    attachInterruptArg(KEY3_PIN, handleKey, (void *)KEY3_PIN, CHANGE);

    pinMode(9, INPUT_PULLUP);
    while (1)
    {
        if (!digitalRead(9))
        {
            blue::pressChar('a');
            delay(100);
            blue::releaseChar();
        }
        delay(100);
    }
}