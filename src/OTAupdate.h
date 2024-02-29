#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>

// #include <AsyncElegantOTA.h>
#include <ESPUI.h>
#include <Update.h>

/*--------------------------------------------------------------------------------*/
/*      O T A    U P D A T E                                                      */
/*--------------------------------------------------------------------------------*/

class cOTAupdate {

  private:

    static void update(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final);

    static void initTask(void *pvParameters);

  public:

    void init ();

  };

extern cOTAupdate oOTAupdate;