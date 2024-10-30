#pragma once
#include <Arduino.h>
#include <stdint.h>

#include "config.h"
namespace PowerMon
{
    inline void init()
    {
        analogSetAttenuation(ADC_11db);
    }
    inline uint32_t getVoltage()
    {
        auto rawVoltage = analogReadMilliVolts(VBAT_SENS_PIN);
        return rawVoltage * VBAT_DIV_COEFF;
    }

    inline uint8_t getBatteryLevel()
    {
        auto voltage = getVoltage();
        ESP_LOGD("PowerMon", "Voltage: %ld", voltage);
        
        if (voltage > 4200)
            return 100;
        else if (voltage < 3000)
            return 0;

        return map(voltage, 3000, 4200, 0, 100);
    }
} // namespace PowerMon
