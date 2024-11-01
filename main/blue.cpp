#include <memory>
#include <NimBLEDevice.h>
#include <NimBLECharacteristic.h>
#include <NimBLEHIDDevice.h>
#include <Preferences.h>
#include "blue.h"

#include "config.h"

static uint8_t hidMap[] = {
    USAGE_PAGE(1), 0x01, // USAGE_PAGE (Generic Desktop Ctrls)
    USAGE(1), 0x06,      // USAGE (Keyboard)
    COLLECTION(1), 0x01, // COLLECTION (Application)
    // ------------------------------------------------- Keyboard
    REPORT_ID(1), REPORT_ID_KEYBOARD, //   REPORT_ID (1)
    USAGE_PAGE(1), 0x07,              //   USAGE_PAGE (Kbrd/Keypad)
    USAGE_MINIMUM(1), 0xE0,           //   USAGE_MINIMUM (0xE0)
    USAGE_MAXIMUM(1), 0xE7,           //   USAGE_MAXIMUM (0xE7)
    LOGICAL_MINIMUM(1), 0x00,         //   LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1), 0x01,         //   Logical Maximum (1)
    REPORT_SIZE(1), 0x01,             //   REPORT_SIZE (1)
    REPORT_COUNT(1), 0x08,            //   REPORT_COUNT (8)
    HIDINPUT(1), 0x02,                //   INPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    REPORT_COUNT(1), 0x01,            //   REPORT_COUNT (1) ; 1 byte (Reserved)
    REPORT_SIZE(1), 0x08,             //   REPORT_SIZE (8)
    HIDINPUT(1), 0x01,                //   INPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    REPORT_COUNT(1), 0x05,            //   REPORT_COUNT (5) ; 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
    REPORT_SIZE(1), 0x01,             //   REPORT_SIZE (1)
    USAGE_PAGE(1), 0x08,              //   USAGE_PAGE (LEDs)
    USAGE_MINIMUM(1), 0x01,           //   USAGE_MINIMUM (0x01) ; Num Lock
    USAGE_MAXIMUM(1), 0x05,           //   USAGE_MAXIMUM (0x05) ; Kana
    HIDOUTPUT(1), 0x02,               //   OUTPUT (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    REPORT_COUNT(1), 0x01,            //   REPORT_COUNT (1) ; 3 bits (Padding)
    REPORT_SIZE(1), 0x03,             //   REPORT_SIZE (3)
    HIDOUTPUT(1), 0x01,               //   OUTPUT (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    REPORT_COUNT(1), 0x06,            //   REPORT_COUNT (6) ; 6 bytes (Keys)
    REPORT_SIZE(1), 0x08,             //   REPORT_SIZE(8)
    LOGICAL_MINIMUM(1), 0x00,         //   LOGICAL_MINIMUM(0)
    LOGICAL_MAXIMUM(1), 0x65,         //   LOGICAL_MAXIMUM(0x65) ; 101 keys
    USAGE_PAGE(1), 0x07,              //   USAGE_PAGE (Kbrd/Keypad)
    USAGE_MINIMUM(1), 0x00,           //   USAGE_MINIMUM (0)
    USAGE_MAXIMUM(1), 0x65,           //   USAGE_MAXIMUM (0x65)
    HIDINPUT(1), 0x00,                //   INPUT (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    END_COLLECTION(0),                // END_COLLECTION
};

namespace blue
{
    static Preferences pref;
    // static BLECharacteristic *pTxCharacteristic;
    // static bool deviceConnected = false;
    static std::shared_ptr<NimBLEHIDDevice> keyboard;
    static NimBLEServer *pServer;
    static std::shared_ptr<NimBLECharacteristic> inputReport;

    static advCompleteCB_t advCompleteCB = nullptr;
    struct KeyConfig
    {
        bool isModifier;
        uint8_t code; // ASCII if not modifier
    };
    static KeyConfig keyConfigs[KEY_COUNT];

    // class SvrCallbacks : public BLEServerCallbacks
    // {
    //     void onConnect(BLEServer *pServer, BLEConnInfo &connInfo)
    //     {
    //         pServer->stopAdvertising();
    //         deviceConnected = true;
    //     };

    //     void onDisconnect(BLEServer *pServer, BLEConnInfo &connInfo, int reason)
    //     {
    //         pServer->startAdvertising();
    //         deviceConnected = false;
    //     }
    // };

    class CharCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite(BLECharacteristic *pCharacteristic, BLEConnInfo &connInfo)
        {
            std::string rxValue = pCharacteristic->getValue();

            if (rxValue.length() > 0)
            {
                printf("Received: %s\n", rxValue.c_str());
                pref.putString("name", rxValue.c_str());
            }
        }
    };

    bool isConnected()
    {
        return pServer->getConnectedCount();
    }

    void init(advCompleteCB_t cb)
    {
        pref.begin("keyboard", false);

        auto devName = pref.getString("name", DEFAULT_NAME);
        keyConfigs[0] = {pref.getBool("is_modifier0", true), pref.getUChar("code0", 0)};
        keyConfigs[1] = {pref.getBool("is_modifier1", false), pref.getUChar("code1", 'c')};
        keyConfigs[2] = {pref.getBool("is_modifier2", false), pref.getUChar("code2", 'v')};

        NimBLEDevice::init(devName.c_str());

        pServer = NimBLEDevice::createServer();
        // pServer->setCallbacks(new SvrCallbacks());

        keyboard = std::make_shared<NimBLEHIDDevice>(pServer);
        keyboard->manufacturer("RedStoneeeTek");
        keyboard->pnp(0x02, 0xe502, 0xa111, 0x0210); // v0.1
        keyboard->hidInfo(0x00, 0x01);               // 1=Keyboard, 2=Mouse
        keyboard->reportMap(hidMap, sizeof(hidMap));
        inputReport = std::shared_ptr<NimBLECharacteristic>(keyboard->inputReport(REPORT_ID_KEYBOARD));
        keyboard->outputReport(1);
        keyboard->startServices();

        // Start advertising
        pServer->getAdvertising()->setAppearance(HID_KEYBOARD);
        pServer->getAdvertising()->addServiceUUID(keyboard->hidService()->getUUID());
        pServer->getAdvertising()->addServiceUUID(keyboard->batteryService()->getUUID());
        pServer->getAdvertising()->addServiceUUID(keyboard->deviceInfo()->getUUID());
        advCompleteCB = cb;
        pServer->getAdvertising()->start(CONNECT_TIMEOUT, cb);

        ESP_LOGI("BLE", "BLE initialized");
        printf("Waiting a client connection to notify...\n");
    }

    void deinit()
    {
        NimBLEDevice::deinit(false);
    }

    void setBatteryLevel(uint8_t level)
    {
        keyboard->setBatteryLevel(level);
    }

    uint8_t hidData[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    void sendKeys()
    {
        inputReport->setValue(hidData, 8);
        inputReport->notify();
    }
    void pressModifier(MODIFIER_KEY key)
    {
        hidData[0] = key;
        sendKeys();
    }

    void releaseModifier()
    {
        hidData[0] = 0;
        sendKeys();
    }

    void releaseAllKeys()
    {
        hidData[0] = 0;
        hidData[2] = 0;
        hidData[4] = 0;
        hidData[6] = 0;
        sendKeys();
    }

    void pressChar(char ch)
    {
        hidData[0] |= keymap[static_cast<uint8_t>(ch)].modifier;
        hidData[0] &= keymap[static_cast<uint8_t>(ch)].modifier;

        hidData[2] = keymap[static_cast<uint8_t>(ch)].usage;
        sendKeys();
    }

    void releaseChar()
    {
        hidData[2] = 0;
        sendKeys();
    }

    void onKeyStatusChange(uint8_t key, bool isPressed)
    {
        if (key < KEY_COUNT)
            if (keyConfigs[key].isModifier)
                if (isPressed)
                    pressModifier(static_cast<MODIFIER_KEY>(keyConfigs[key].code));
                else
                    releaseModifier();

            else if (isPressed)
                pressChar(keyConfigs[key].code);
            else
                releaseChar();
    }

} // namespace blue
