#include "BLEServerService.h"

/*--------------------------------------------------------------------------------*/

// BLE Server
BLEServer* pBLEServer;

static BLECharacteristic* hrmServerCharacteristic;
static BLECharacteristic* posServerCharacteristic;
static BLECharacteristic* batServerCharacteristic;

/*--------------------------------------------------------------------------------*/
/*      B L E    S E R V E R                                                      */
/*--------------------------------------------------------------------------------*/

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pBLEServer) {

        Log.trace(F("[%l] [BLEserver] clientMode: %d, serverMode: %d\r\n"),  millis(), clientMode, serverMode);

        if(clientMode != MODE_NOTIFY) { // clientMode from BLE client *not* server
            Log.trace(F("[%l] [BLEserver] Server ignore 1.\r\n"),  millis());
        }
        else if (clientMode == MODE_NOTIFY && serverMode == MODE_WAIT) {
            Log.notice(F("[%l] [BLEserver] Client connected\r\n"),  millis());
            serverMode = MODE_NOTIFY;
        }
        else {
            Log.trace(F("[%l] [BLEserver] Server ignore 2.\r\n"),  millis());
        }
    }

    void onDisconnect(BLEServer* pBLEServer) {
    Log.notice(F("[%l] [BLEserver] Client disconnected\r\n"),  millis());
    serverMode = MODE_WAIT;
    }
};

uint32_t cBLEServerService::getchipid () {

    uint32_t chipid = 0;
    for (int i = 0; i < 17; i = i + 8)
    {
        chipid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    return chipid;
}

void cBLEServerService::initTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        // wait for client connection to HRM before starting server
        while(clientMode != MODE_NOTIFY)
        {
            Log.notice(F("[%l] [BLEserver] Waiting to start...\r\n"),  millis());
            vTaskDelay( scanTime*1000 / portTICK_PERIOD_MS );
        }
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
        
        Log.notice(F("[%l] [BLEserver] Starting...\r\n"),  millis());

        // set BLE HRM name
        snprintf(BLEServerName, 26, "WENDEEE HRM %08X", cBLEServerService::getchipid());
        esp_ble_gap_set_device_name(BLEServerName);

        BLEDevice::init(BLEServerName);

        Log.notice(F("[%l] [BLEserver] Name: %s\r\n"),  millis(), BLEServerName);

        // Create the BLE Server
        pBLEServer = BLEDevice::createServer();
        pBLEServer->setCallbacks(new MyServerCallbacks());

        // Create the BLE Service (for HRM)
        BLEService *phrmService = pBLEServer->createService(hrmServUUID);

        // Create a BLE Characteristic
        hrmServerCharacteristic = phrmService->createCharacteristic(hrmCharUUID,BLECharacteristic::PROPERTY_NOTIFY);
        posServerCharacteristic = phrmService->createCharacteristic(posCharUUID,BLECharacteristic::PROPERTY_READ);

        // Create a BLE Descriptor
        hrmServerCharacteristic->addDescriptor(new BLE2902());

        // Create the BLE Service (for battery)
        BLEService *pbatService = pBLEServer->createService(batServUUID);

        // Create a BLE Characteristic
        batServerCharacteristic = pbatService->createCharacteristic(batCharUUID,BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        // Create a BLE Descriptor
        batServerCharacteristic->addDescriptor(new BLE2902());

        posServerCharacteristic->setValue(hrmPos, 1); // position
        batServerCharacteristic->setValue(hrmBat, 1); // battery

        // Start the service(s)
        phrmService->start();
        pbatService->start();

        // Start advertising
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(hrmServUUID);
        pAdvertising->addServiceUUID(batServUUID);
        // pAdvertising->setScanResponse(false);
        // pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
        pAdvertising->setMinPreferred(0x12);

        Log.notice(F("[%l] [BLEserver] Start advertising...\r\n"),  millis());
        BLEDevice::startAdvertising();
        Log.notice(F("[%l] [BLEserver] Waiting a client connection to notify...\r\n"),  millis());

        // Server enabled
        serverMode = MODE_WAIT;

        vTaskDelete( NULL ); // end task

    }
}

void cBLEServerService::serverTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        // Perform tasks
        switch (serverMode) {
                        
            case MODE_NOTIFY: { 
                Log.verbose(F("[%l] [BLEserver] Notifying Heart Rate: %d BPM\r\n"),  millis(),hrmData[1]);
                hrmServerCharacteristic->setValue(hrmData, 8);
                hrmServerCharacteristic->notify();
                vTaskDelay( hrmTime / portTICK_PERIOD_MS ); // bluetooth stack will go into congestion, if too many packets are sent
                                                            // in 6 hours test i was able to go as low as 3ms
                break;
            }

            case MODE_DISCONNECT: { // Disconnect from device
                // pBLEServer->disconnect(); // not working
                Log.notice(F("[%l] [BLEserver] Force disconnect\r\n"),  millis());
                serverMode = MODE_WAIT;
                vTaskDelay( 1000 / portTICK_PERIOD_MS );
                break;
            }

            case MODE_WAIT: {
                Log.notice(F("[%l] [BLEserver] Waiting %d seconds to readvertise\r\n"),  millis(),rstTime);
                vTaskDelay(rstTime*1000 / portTICK_PERIOD_MS );
                Log.notice(F("[%l] [BLEserver] Readvertising...\r\n"),  millis(),rstTime);
                pBLEServer->startAdvertising(); // restart advertising
                break;
            }

            case MODE_OFF: {
                Log.verbose(F("[%l] [BLEserver] OFF mode\r\n"),  millis(),rstTime);
                vTaskDelay(1000 / portTICK_PERIOD_MS );
                break;
            }

            default: {
                Log.error(F("[%l] [BLEserver] Error in serverMode %d value\r\n"),  millis(),serverMode);
                vTaskDelay( 1000 / portTICK_PERIOD_MS );
                break;
            }
        }     

    }
}

void cBLEServerService::init() {
    xTaskCreate(cBLEServerService::initTask,"initTask",4096,NULL,BLEserverPriority,NULL);
}

void cBLEServerService::server() {
    xTaskCreate(cBLEServerService::serverTask,"serverTask",4096,NULL,BLEserverPriority,NULL);
}