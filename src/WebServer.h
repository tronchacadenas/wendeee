#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>
#include <Preferences.h>

// widgets
#include <LittleFS.h>
#include <ESPUI.h>

/*--------------------------------------------------------------------------------*/
/*      W E B    S E R V E R                                                      */
/*--------------------------------------------------------------------------------*/

class cWebServer {

  TaskHandle_t webHandle = NULL;

  private:
  
    static void rebootCall(Control* sender, int type);

    static void resetCall(Control* sender, int type);

    static void yesCall(Control* sender, int type);

    static void noCall(Control* sender, int type);
    
    static void clientBondCall(Control* sender, int type);

    static void disconnectCall(Control* sender, int type);

    static void ssidCall(Control* sender, int type);

    static void passwordCall(Control* sender, int type);

    static void connectCall(Control* sender, int type);

    static void wifiAutoCall(Control* sender, int type);

    static void fanAutoCall(Control* sender, int type);

    static void fanSpeedCall(Control* sender, int type);

    static void min_fanCall(Control* sender, int type);

    static void max_fanCall(Control* sender, int type);

    static void min_hrmCall(Control* sender, int type);

    static void max_hrmCall(Control* sender, int type);
    
    static void webUpdate();
    
    static void webInterface();
    
    static void initTask(void *pvParameters);

    static void webTask(void *pvParameters);

  public:

    // void webInterface();

    void init ();

    void update();

};

extern cWebServer oWebServer;