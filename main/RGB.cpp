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

    
} // namespace RGB
