#pragma once

#include <Arduino.h>
#include "globals.h"

#include <ArduinoLog.h>

// AC dimmable Fan
#include <dimmable_light.h>

/*--------------------------------------------------------------------------------*/
/*      F A N    C O N T R O L                                                    */
/*--------------------------------------------------------------------------------*/

class cFanControl {

  TaskHandle_t controlHandle = NULL;

  private:

    static void controlTask(void *pvParameters);

  public:

    void init ();

    void control();

};

extern cFanControl oFanControl;