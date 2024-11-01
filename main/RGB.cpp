#include <Adafruit_NeoPixel.h>

#include "config.h"
#include "RGB.h"

namespace RGB
{
    Adafruit_NeoPixel bulbs(PIXEL_COUNT, PIXEL_PIN);
    void init()
    {
        bulbs.begin();
        bulbs.clear();
        bulbs.setBrightness(100);
        bulbs.show();
        ESP_LOGI("RGB", "Initialized");
    }

    void test(void *pvParameters)
    {
        bulbs.fill(bulbs.Color(255, 0, 0), 0, PIXEL_COUNT);
        bulbs.show();
        delay(500);
        bulbs.fill(bulbs.Color(0, 255, 0), 0, PIXEL_COUNT);
        bulbs.show();
        delay(500);
        bulbs.fill(bulbs.Color(0, 0, 255), 0, PIXEL_COUNT);
        bulbs.show();
        delay(500);
        bulbs.fill(bulbs.Color(255, 255, 255), 0, PIXEL_COUNT);
        bulbs.show();
        delay(500);
        bulbs.clear();
        bulbs.show();
        vTaskDelete(NULL);
    }

} // namespace RGB
