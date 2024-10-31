#include <Arduino.h>
#include <NimBLEHIDDevice.h>
#include <HIDKeyboardTypes.h>

namespace blue
{
    void init(advCompleteCB_t cb);
    void deinit();
    bool isConnected();
    void setBatteryLevel(uint8_t level);

    void releaseAllKeys();

    void pressModifier(MODIFIER_KEY key);
    void releaseModifier();

    void pressChar(char ch);
    void releaseChar();

    void onKeyStatusChange(uint8_t key, bool isPressed);
}