#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>

// bluetooth
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

/*--------------------------------------------------------------------------------*/
/*      B L E    C L I E N T                                                      */
/*--------------------------------------------------------------------------------*/

class cBLEClientService {

  TaskHandle_t scanHandle = NULL;

  private:

    // BLE Heart Rate Measure Callback
    static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                                uint8_t* pData, size_t length, bool isNotify);

    static bool connectToServer();

    static void clientTask(void *pvParameters);

  public:

    void init();

    void client();

};

extern cBLEClientService oBLEClientService;