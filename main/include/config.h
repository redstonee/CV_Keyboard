#pragma once
#include "stdint.h"

enum Keys
{
    KEY1_PIN = 4,
    KEY2_PIN = 3,
    KEY3_PIN = 2,
    KEY_COUNT
};

constexpr uint8_t PIXEL_PIN = 5;
constexpr uint8_t PIXEL_COUNT = 3;
// constexpr uint8_t

constexpr uint8_t VBAT_SENS_PIN = 1;
constexpr float VBAT_DIV_COEFF = (10.0 + 22.0) / 22;

constexpr uint32_t CONNECT_TIMEOUT = 5000;
constexpr auto DEFAULT_NAME = "CV Keyboard";