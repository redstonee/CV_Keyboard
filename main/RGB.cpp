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

    void waitForPairingTask(void *param)
    {
        while (1)
        {
            for (uint8_t i = 0; i < 255; i += 5)
            {
                bulbs.fill(bulbs.ColorHSV(10000, 200, i), 0, PIXEL_COUNT);
                bulbs.show();
                delay(20);
            }
            for (uint8_t i = 255; i > 0; i -= 5)
            {
                bulbs.fill(bulbs.ColorHSV(10000, 200, i), 0, PIXEL_COUNT);
                bulbs.show();
                delay(20);
            }
        }
    }

    void waitForReconnectTask(void *param)
    {
    }

    void fadeIn(uint8_t pixel)
    {
        for (uint8_t i = 255; i > 0; i -= 5)
        {
            bulbs.setPixelColor(pixel, bulbs.ColorHSV(esp_random(), 255, i));
            bulbs.show();
            delay(20);
        }
    }

    void fuckOff()
    {
        bulbs.clear();
        bulbs.show();
    }
} // namespace RGB
