// include libraries
#include <Arduino.h>
#include "globals.h"
#include "main.h"

/*--------------------------------------------------------------------------------*/

void upload(void)
{
  ESPUI.setVerbosity(Verbosity::Verbose); //Enable verbose output so you see the files in LittleFS
  delay(500); //Delay to allow Serial Monitor to start after a reset
  Serial.println(F("\nPreparing filesystem with ESPUI resources"));
  ESPUI.prepareFileSystem();  //Copy across current version of ESPUI resources
  Serial.println(F("Done, files..."));
  ESPUI.list(); //List all files on LittleFS, for info
}

/*--------------------------------------------------------------------------------*/
/*      S E R I A L                                                               */
/*--------------------------------------------------------------------------------*/

void init()
{
  Serial.begin(BAUD);
  Log.begin(ARDUINO_LOG_LEVEL, &Serial, true);
  delay(2000);
}



/*--------------------------------------------------------------------------------*/
/*      M A I N                                                                   */
/*--------------------------------------------------------------------------------*/

void setup() { // main function.
  
  init();                     // init (serial)

  oStorage.init();            // read init values
  
  oWifi.init();               // init WiFi
  oWifi.services();           // start WiFi AP & STA

  oBLEClientService.init();   // init BLE Client (scanning)
  oBLEClientService.client(); // start BLE Client (pair & subscribe)

  oFanControl.init();         // init Fan Controller

  oBLEServerService.init();   // init BLE server (announce)
  oBLEServerService.server(); // start BLE server (connect & notify)
  
  oFanControl.control();      // start Fan Controller with HRM

  oWebServer.init();          // init web server UI
  oWebServer.update();        // start web server updates (periodic and event based)
  
  oStorage.save();            // start storage task (periodic)

  oOTAupdate.init();          // init OTA update client

}

/*--------------------------------------------------------------------------------*/
/*      L O O P                                                                   */
/*--------------------------------------------------------------------------------*/

void loop() {} // loop function.

/*--------------------------------------------------------------------------------*/
