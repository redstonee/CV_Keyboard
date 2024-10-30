#include <Arduino.h>
#include <NimBLEHIDDevice.h>
#include <HIDKeyboardTypes.h>
namespace blue
{
    void init();
    void releaseAllKeys();
    void pressModifier(MODIFIER_KEY key);
    void releaseModifier();
    void pressChar(char ch);
    void releaseChar();
    bool isConnected();
    void setBatteryLevel(uint8_t level);
}