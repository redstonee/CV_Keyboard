idf_component_register(
    SRCS "hid2ble/BleConnectionStatus.cpp" "hid2ble/Hid2Ble.cpp" "hid2ble/KeyboardOutputCallbacks.cpp"
    INCLUDE_DIRS "hid2ble"
    REQUIRES "nimble" "arduino"
)
