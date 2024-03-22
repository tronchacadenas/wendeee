#include "globals.h"

/*--------------------------------------------------------------------------------*/

// classes for each service

cWebServer        oWebServer;
cBLEClientService oBLEClientService;
cBLEServerService oBLEServerService;
cWifi             oWifi;
cFanControl       oFanControl;
cStorage          oStorage;
cOTAupdate        oOTAupdate;

/*--------------------------------------------------------------------------------*/

// BLE Client & Server
BLEUUID hrmServUUID(BLEUUID((uint16_t)0x180D)); // BLE Heart Rate Service
BLEUUID hrmCharUUID(BLEUUID((uint16_t)0x2A37)); // BLE Heart Rate Monitor Characteristic
BLEUUID posCharUUID(BLEUUID((uint16_t)0x2A38)); // BLE Heart Rate Position Characteristic

BLEUUID batServUUID(BLEUUID((uint16_t)0x180F)); // BLE Heart Rate Battery Service
BLEUUID batCharUUID(BLEUUID((uint16_t)0x2A19)); // BLE Heart Rate Battery Characteristic

/*--------------------------------------------------------------------------------*/

// BLE Client
int clientMode = MODE_OFF;

String clientName;
String clientAddress;

// Client bonding
boolean clientBond = false;  // true if bond to specific UUID address
String  clientAddressBond;   // BLE Heart Rate Monitor Address to bond to (if selected)

// BLE Server
int serverMode = MODE_OFF;
char BLEServerName[25];

/*--------------------------------------------------------------------------------*/

// HRM Data
byte hrmData[8] =  { 0, 0, 0, 0, 0 , 0, 0, 0};
byte hrmPos[1] = {1};   // chest position
byte hrmBat[1] = {100}; // 100% battery

/*--------------------------------------------------------------------------------*/

// Fan speed control
bool fanAuto = true;

int min_fan =   0;
int max_fan = 100;
int min_hrm =  40;
int max_hrm = 160;

int fanSpeed = 0;
int fanSpeed_offset = 100;

/*--------------------------------------------------------------------------------*/

// WiFi
IPAddress ip(192, 168, 4, 1);
IPAddress gateway = ip;
IPAddress subnet(255,255,255,0);

bool wifiAuto = true;

String wifiSSID;
String wifiPassword;

String hostname = "wendeee";

bool wifiClientReady = false;
bool wifiServerReady = false;

/*--------------------------------------------------------------------------------*/
// Webserver
bool ESPUIready = false;

/*--------------------------------------------------------------------------------*/