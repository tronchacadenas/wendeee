#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>
#include <Preferences.h>

/*--------------------------------------------------------------------------------*/
/*      E E P R O M                                                               */
/*--------------------------------------------------------------------------------*/

class cStorage {

  private:

  static void control();
  
  static void saveTask(void *pvParameters);

  public:

    void init();

    void ble();

    void wifi();

    void reset();

    void save();

};

extern cStorage oStorage;