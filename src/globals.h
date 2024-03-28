#pragma once

#include <Arduino.h>
#include <Preferences.h>

// bluetooth
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>

// classes
#include "WebServer.h"
#include "WifiService.h"
#include "FanControl.h"
#include "BLEClientService.h"
#include "BLEServerService.h"
#include "Storage.h"
#include "OTAupdate.h"

/*--------------------------------------------------------------------------------*/

#define VERSION_SW "0.9.10 Beta"

#define ARDUINO_LOG_LEVEL 5

#define storagePriority   11  // Save control values periodic task
#define BLEclientPriority 10  // BLE client (always higher)
#define BLEserverPriority  9  // BLE server
#define controlPriority    8  // FAN control
#define wifiPriority       7  // Wifi
#define webPriority        6  // Web server


#define rstTime           10  // Time between servicies resets (seconds)
#define scanTime           5  // Client Scan Duration (seconds)
#define hrmTime          500  // HRM notification frequency (miliseconds)
#define controlTime        1  // Fan control frequency (seconds)
#define APTime            15  // Timeout to start wifi AP (seconds)
#define RefreshTime        1  // Web auto refresh frequency (seconds)
#define SaveTime          30  // Control auto refresh frequency (seconds)

/*--------------------------------------------------------------------------------*/

// BLE Client & Server
extern BLEUUID hrmServUUID;
extern BLEUUID hrmCharUUID;
extern BLEUUID posCharUUID;
extern BLEUUID batServUUID;
extern BLEUUID batCharUUID;

#define MODE_OFF        0
#define MODE_WAIT       1
#define MODE_SCAN       2
#define MODE_PAIR       3
#define MODE_NOTIFY     4
#define MODE_DISCONNECT 5

/*--------------------------------------------------------------------------------*/

// BLE Client
extern int clientMode;

extern String clientName;
extern String clientAddress;

// Client bonding
extern boolean clientBond;
extern String  clientAddressBond;

// BLE Server
extern int serverMode;
extern char BLEServerName[25];

/*--------------------------------------------------------------------------------*/

// HRM Data
extern byte hrmData[8];
extern byte hrmPos[1];
extern byte hrmBat[1];

/*--------------------------------------------------------------------------------*/

// Fan speed control
#define NETWORK_FREQ_FIXED_50HZ
#define FILTER_INT_PERIOD
#define PRINT_INT_PERIOD

#define outputPin  2
#define zerocross  1

extern bool fanAuto;

extern int min_fan;
extern int max_fan;
extern int min_hrm;
extern int max_hrm;

extern int fanSpeed;
extern int fanSpeed_offset;

/*--------------------------------------------------------------------------------*/

// WiFi
extern IPAddress ip;
extern IPAddress gateway;
extern IPAddress subnet;

extern bool wifiAuto;

extern String wifiSSID;
extern String wifiPassword;

extern String hostname;

extern bool wifiClientReady;
extern int  wifiClientRetry;
extern bool wifiServerReady;

/*--------------------------------------------------------------------------------*/

extern bool ESPUIready;

/*--------------------------------------------------------------------------------*/

extern Preferences preferences;

/*--------------------------------------------------------------------------------*/