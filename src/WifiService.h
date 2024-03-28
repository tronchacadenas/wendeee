#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>
#include <Preferences.h>

// wifi, dns server
#include <ESPmDNS.h>
#include <WiFi.h>

/*-------------------------------------------------------------------*/
/*      W I F I                                                      */
/*-------------------------------------------------------------------*/

class cWifi {

  TaskHandle_t wifiHandle = NULL;
  
  private:

    static void mode();

    static void connect();

    static void disconnect();

    static void create();

    static void infoAP();

    static void infoSTA();

    static void mdns();

    static void wifiEvent(WiFiEvent_t event);

    static uint32_t getchipid ();

    static void wifiTask(void *pvParameters);

  public:

    void init () ;

    void services();

};

extern cWifi oWifi;