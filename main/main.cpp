#include "Arduino.h"
#include <Hid2Ble.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLECharacteristic.h"
#include <Adafruit_NeoPixel.h>

extern "C" void app_main()
{
    initArduino();

}