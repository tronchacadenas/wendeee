#include "BLEClientService.h"

/*--------------------------------------------------------------------------------*/

BLEScan*   pBLEScan;
BLEClient* pBLEClient;

static BLEAdvertisedDevice*     hrmClientDevice;
static BLERemoteCharacteristic* hrmClientCharacteristic;

/*--------------------------------------------------------------------------------*/
/*      B L E    C L I E N T                                                      */
/*--------------------------------------------------------------------------------*/

class MyClientCallbacks : public BLEClientCallbacks {
    void onConnect(BLEClient* pBLEClient) {
    Log.notice(F("[%l] [BLEclient] Connecting...\r\n"),  millis());
    }

    void onDisconnect(BLEClient* pBLEClient) {
    Log.notice(F("[%l] [BLEclient] Disconnected: %s\r\n"),  millis(), pBLEClient->getPeerAddress().toString().c_str());
    clientMode = MODE_SCAN;
    }
};

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
    if ( (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(hrmServUUID)) 
        && (!clientBond || advertisedDevice.getAddress().toString() == clientAddressBond.c_str()) ) {
        Log.notice(F("[%l] [BLEclient] %s\r\n"),  millis(), advertisedDevice.toString().c_str());
        BLEDevice::getScan()->stop();
        hrmClientDevice = new BLEAdvertisedDevice(advertisedDevice);
        clientMode = MODE_PAIR;

        // save for web use
        clientName    = advertisedDevice.getName().c_str();
        clientAddress = advertisedDevice.getAddress().toString().c_str();
    } 
    }
};

// BLE Heart Rate Measure Callback
void cBLEClientService::notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData, size_t length, bool isNotify) {
    if (length) {
    uint16_t heart_rate_measurement = pData[1];
    // if (pData[0] & 1) {
    //   heart_rate_measurement += (pData[2] << 8);
    // }

    // copy data
    for (int i = 1; i <= length; i++) { hrmData[i]=pData[i]; }

    Log.verbose(F("[%l] [BLEclient] Heart Rate: %d BPM\r\n"),  millis(), heart_rate_measurement);
    }
}

bool cBLEClientService::connectToServer() {
    
    Log.notice(F("[%l] [BLEclient] Forming a connection to: %s\r\n"),  millis(), hrmClientDevice->getAddress().toString().c_str());
    
    // Create BLE Client.
    pBLEClient = BLEDevice::createClient();
    Log.notice(F("[%l] [BLEclient] Created client\r\n"),  millis());

    // Connect to the remote BLE Server.
    pBLEClient->setClientCallbacks(new MyClientCallbacks());
    pBLEClient->connect(hrmClientDevice);
    // if you pass BLEAdvertisedDevice instead of address,
    // it will be recognized type of peer device address (public or private)
    Log.notice(F("[%l] [BLEclient] Connected to server: %s\r\n"),  millis(), pBLEClient->getPeerAddress().toString().c_str());

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pBLEClient->getService(hrmServUUID);
    if (pRemoteService == nullptr) {
    Log.error(F("[%l] [BLEclient] Failed to find our service UUID: %s\r\n"),  millis(), hrmServUUID.toString().c_str());
    pBLEClient->disconnect();
    return false;
    }
    Log.notice(F("[%l] [BLEclient] Found our service UUID: %s\r\n"),  millis(), hrmServUUID.toString().c_str());

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    hrmClientCharacteristic = pRemoteService->getCharacteristic(hrmCharUUID);
    if (hrmClientCharacteristic == nullptr) {
    Log.error(F("[%l] [BLEclient] Failed to find our characteristic UUID: %s\r\n"),  millis(), hrmCharUUID.toString().c_str());
    pBLEClient->disconnect();
    return false;
    }
    Log.notice(F("[%l] [BLEclient] Found our characteristic UUID: %s\r\n"),  millis(), hrmCharUUID.toString().c_str());

    // Register to server.
    if (hrmClientCharacteristic->canNotify()) {
    hrmClientCharacteristic->registerForNotify(notifyCallback);
    } else {
    Log.error(F("[%l] [BLEclient] Failed to register to server: %s\r\n"),  millis(), pBLEClient->getPeerAddress().toString().c_str());
    pBLEClient->disconnect();
    return false;
    }
    Log.notice(F("[%l] [BLEclient] Registered to server: %s\r\n"),  millis(), pBLEClient->getPeerAddress().toString().c_str());

    clientMode = MODE_NOTIFY;
    return true;
}

void cBLEClientService::clientTask(void *pvParameters) {

    (void)pvParameters;

    while (1) { // A Task shall never return or exit.

        // Perform tasks
        switch (clientMode) {
            
            case MODE_SCAN: { // Enter scanning mode
                Log.notice(F("[%l] [BLEclient] Performing Scan...\r\n"),  millis());
                BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
                Log.notice(F("[%l] [BLEclient] Found %d devices\r\n"),  millis(),foundDevices.getCount());
                pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
                Log.notice(F("[%l] [BLEclient] Scan done\r\n"),  millis());
                vTaskDelay(scanTime*1000 / portTICK_PERIOD_MS );
                break;
            }

            case MODE_PAIR: { // Enter pairing mode
                if (!connectToServer()) {
                    Log.error(F("[%l] [BLEclient] Failed to connect to HRM\r\n"),  millis());
                }
                vTaskDelay( 10 / portTICK_PERIOD_MS );
                break;
            }

            case MODE_DISCONNECT: { // Disconnect from device
                pBLEClient->disconnect();
                Log.notice(F("[%l] [BLEclient] Disconnect\r\n"),  millis());
                vTaskDelay( 10 / portTICK_PERIOD_MS );
                clientMode = MODE_SCAN;
                break;
            }
            
            case MODE_NOTIFY: { // Already notified. Do nothing (wait)
                Log.trace(F("[%l] [BLEclient] Sleeping for %d seconds...\r\n"),  millis(),rstTime);
                vTaskDelay(rstTime*1000 / portTICK_PERIOD_MS );
                break;
            }

            case MODE_OFF: {
                Log.verbose(F("[%l] [BLEclient] OFF mode\r\n"),  millis(),rstTime);
                vTaskDelay(1000 / portTICK_PERIOD_MS );
                break;
            }

            
            default: {
                Log.error(F("[%l] [BLEclient] Error in clientMode %d value\r\n"),  millis(),clientMode);
                vTaskDelay( 1000 / portTICK_PERIOD_MS );
                break;
            }
        }        
    }
}

void cBLEClientService::init() {

    Log.notice(F("[%l] [BLEclient] Starting...\r\n"),  millis());
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value

    // Client enabled
    clientMode = MODE_SCAN;
    
}

void cBLEClientService::client() {
    xTaskCreate(cBLEClientService::clientTask,"clientTask",4096,NULL,BLEclientPriority,NULL);
}