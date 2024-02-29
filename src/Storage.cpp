#include "Storage.h"

/*--------------------------------------------------------------------------------*/

Preferences preferences;

/*--------------------------------------------------------------------------------*/
/*      E E P R O M                                                               */
/*--------------------------------------------------------------------------------*/

void cStorage::control () {

    Log.notice(F("[%l] [eeprom] Storing control info to memory (conditionally) ... \r\n"),  millis());

    preferences.begin("control", false);
    if (preferences.getInt("min_fan",0) != min_fan) preferences.putInt("min_fan", min_fan);
    if (preferences.getInt("max_fan",0) != max_fan) preferences.putInt("max_fan", max_fan);
    if (preferences.getInt("min_hrm",0) != min_hrm) preferences.putInt("min_hrm", min_hrm);
    if (preferences.getInt("max_hrm",0) != max_hrm) preferences.putInt("max_hrm", max_hrm);
    preferences.end();
}

void cStorage::saveTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        vTaskDelay( SaveTime*1000 / portTICK_PERIOD_MS ); // wait between saves

        Log.verbose(F("[%l] [storage] Saving Control values ...\r\n"),  millis());
        cStorage::control(); // it only saves changes
    }
}

void cStorage::init () {

  Log.notice(F("[%l] [eeprom] Reading data from memory ... \r\n"),  millis());

  // WIFI
  preferences.begin("wifi", true);
  wifiAuto = preferences.getBool("wifiAuto", wifiAuto);
  wifiSSID     = preferences.getString("wifiSSID", "");
  wifiPassword = preferences.getString("wifiPassword", "");
  preferences.end();

  // BLE
  preferences.begin("ble", true);
  clientBond        = preferences.getBool("clientBond", clientBond);
  clientAddressBond = preferences.getString("clientAddress", "");
  preferences.end();

  // HRM & FAN
  preferences.begin("control", true);
  min_fan = preferences.getInt("min_fan", min_fan);
  max_fan = preferences.getInt("max_fan", max_fan);
  min_hrm = preferences.getInt("min_hrm", min_hrm);
  max_hrm = preferences.getInt("max_hrm", max_hrm);
  preferences.end();

}

void cStorage::ble () {
    
    Log.notice(F("[%l] [eeprom] Storing ble info to memory ... \r\n"),  millis());

    preferences.begin("ble", false);
    preferences.putBool("clientBond", clientBond);
    preferences.putString("clientAddress", clientAddress);
    Log.trace(F("[%l] [eeprom] clientAddressBond: %s ... \r\n"),  millis(), preferences.getString("clientAddress", "").c_str());
    preferences.end();
}

void cStorage::wifi () {

    Log.notice(F("[%l] [eeprom] Storing wifi info to memory ... \r\n"),  millis());

    preferences.begin("wifi", false);
    preferences.putBool("wifiAuto", wifiAuto);
    preferences.putString("wifiSSID", wifiSSID);
    preferences.putString("wifiPassword", wifiPassword);
    Log.trace(F("[%l] [eeprom] ssid    : %s ... \r\n"),  millis(), preferences.getString("wifiSSID", "").c_str());
    Log.trace(F("[%l] [eeprom] password: %s ... \r\n"),  millis(), preferences.getString("wifiPassword", "").c_str());
    preferences.end();
}

void cStorage::reset () {

  Log.notice(F("[%l] [eeprom] Cleaning memory ... \r\n"),  millis());

  // WIFI
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();

  // BLE
  preferences.begin("ble", false);
  preferences.clear();
  preferences.end();

  // HRM & FAN
  preferences.begin("control", false);
  preferences.clear();
  preferences.end();

}

void cStorage::save() {
    xTaskCreate(cStorage::saveTask, "saveTask", 4096,NULL,storagePriority,NULL);
}