#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>

// bluetooth
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>


/*--------------------------------------------------------------------------------*/
/*      B L E    S E R V E R                                                      */
/*--------------------------------------------------------------------------------*/

class cBLEServerService {

  TaskHandle_t serverHandle = NULL;

  private:

    static uint32_t getchipid ();

    static void initTask(void *pvParameters) ;

    static void serverTask(void *pvParameters);

  public:

    void init();

    void server();

};

extern cBLEServerService oBLEServerService;