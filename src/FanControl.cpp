#include "FanControl.h"

/*--------------------------------------------------------------------------------*/

DimmableLight fan(outputPin);

/*--------------------------------------------------------------------------------*/
/*      F A N    C O N T R O L                                                    */
/*--------------------------------------------------------------------------------*/

void cFanControl::controlTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

    // control FAN speed

    if (fanAuto==true) {

        // read hrm data
        int hrm=hrmData[1];

        // fan speed control equation
        if (hrm<min_hrm) {      // below min_hrm set to min fan speed
            fanSpeed = min_fan;
        }
        else if (hrm<max_hrm) { // between min_hrm and max_hrm use linear eq for fan speed
            fanSpeed = (max_fan-min_fan)*(hrm-min_hrm)/(max_hrm-min_hrm) + min_fan; // linear
        }
        else {                  // above max_hrm increase linearly up to real max (255)
            fanSpeed = max_fan + 5*(hrm-max_hrm);
            fanSpeed = fanSpeed > 255-fanSpeed_offset ? 255-fanSpeed_offset : fanSpeed; // cap to 255 (after offset)
        }
    }
    
    // set AC dimmer value
    Log.verbose(F("[%l] [control] Fan speed: %d\r\n"),  millis(),fanSpeed+fanSpeed_offset);
    fan.setBrightness(fanSpeed); // setBrightness(0-255);

    vTaskDelay( controlTime*1000 / portTICK_PERIOD_MS ); // fan control frequency
    }
}

void cFanControl::init () {
    Log.notice(F("[%l] [control] Starting...\r\n"),  millis());
    DimmableLight::setSyncPin(zerocross);
    // VERY IMPORTANT: Call this method to activate the library
    DimmableLight::begin();
}

void cFanControl::control() {
    xTaskCreate(cFanControl::controlTask,"controlTask",4096,NULL,controlPriority,&controlHandle);
}
