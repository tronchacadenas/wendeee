#include "WifiService.h"

/*-------------------------------------------------------------------*/
/*      W I F I                                                      */
/*-------------------------------------------------------------------*/

void cWifi::mode() {
    
    // set mode to dual STA + AP
    Log.notice(F("[%l] [wifi] Setting up WiFi ...\r\n"),  millis());
    WiFi.mode(WIFI_AP_STA);

    vTaskDelay( 1000 / portTICK_PERIOD_MS );
}

void cWifi::create() {

    // configure AP (hotspot)
    Log.notice(F("[%l] [wifi] Creating Hotspot ...\r\n"),  millis());

    // set ESSID name
    char ap_ssid[25];
    snprintf(ap_ssid, 26, "WENDEEE-%08X", cWifi::getchipid());
    WiFi.softAP(ap_ssid);

    WiFi.softAPConfig(ip, gateway, subnet); // configure IP
    WiFi.setTxPower(WIFI_POWER_8_5dBm);

    vTaskDelay( 1000 / portTICK_PERIOD_MS );
}

void cWifi::connect() {
    
    // configure STA (client)
    Log.notice(F("[%l] [wifi] Trying to connect to WiFi\r\n"),  millis());
    WiFi.begin(wifiSSID, wifiPassword);
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
}

void cWifi::disconnect() {
    
    // stop STA (client)
    Log.notice(F("[%l] [wifi] Disconnect to WiFi\r\n"),  millis());
    WiFi.disconnect();
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
}

void cWifi::infoAP() {

    Log.notice(F("[%l] [wifi] INFO\r\n"),  millis());
    Log.notice(F("[%l] [wifi] * Mode: Station\r\n"),  millis());
    Log.notice(F("[%l] [wifi] * IP  : %s\r\n"),  millis(), WiFi.softAPIP().toString());
}

void cWifi::infoSTA() {

    Log.notice(F("[%l] [wifi] INFO\r\n"),  millis());
    Log.notice(F("[%l] [wifi] * Mode: Client\r\n"),  millis());
    Log.notice(F("[%l] [wifi] * IP  : %s\r\n"),  millis(), WiFi.localIP().toString());
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
}

void cWifi::mdns() {

    // start DNS server
    Log.notice(F("[%l] [dns] Starting MDNS responder ...\r\n"),  millis());
    if (!MDNS.begin(hostname)) {
    Log.error(F("[%l] [dns] Error setting up MDNS responder\r\n"),  millis());
    }
}

void cWifi::wifiEvent(WiFiEvent_t event) {

    Log.verbose(F("[%l] [wifi] event: %d\r\n"), millis(),event);

    switch (event) {
        case ARDUINO_EVENT_WIFI_READY: 
        Log.trace(F("[%l] [wifi] Interface ready\r\n"), millis());
        break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Log.trace(F("[%l] [wifi] [STA] Disconnected from access point\r\n"), millis());
        if (wifiClientRetry > 0) {
            wifiClientRetry--;
            wifiClientReady = false;
            cWifi::connect();
        }
        else cWifi::disconnect(); // stop trying to connect
        break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Log.trace(F("[%l] [wifi] [STA] Obtained IP\r\n"), millis());
        wifiClientRetry = 10; // reset retries
        wifiClientReady = true;
        cWifi::infoSTA();
        cWifi::mdns();
        break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
        Log.trace(F("[%l] [wifi] [STA] Lost IP\r\n"), millis());
        break;
        case ARDUINO_EVENT_WIFI_AP_START:
        Log.trace(F("[%l] [wifi] [AP] Access point started\r\n"), millis());
        wifiServerReady = true;
        cWifi::infoAP();
        // cWifi::mdns();
        break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
        Log.trace(F("[%l] [wifi] [AP] Access point stoped\r\n"), millis());
        wifiServerReady = false;
        break;
        default: break;
    }
}

uint32_t cWifi::getchipid () {

    uint32_t chipid = 0;
    for (int i = 0; i < 17; i = i + 8)
    {
        chipid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    return chipid;
}

void cWifi::wifiTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        cWifi::mode();
        cWifi::create();
        cWifi::connect();

        vTaskDelete( NULL ); // end task
    }
}

void cWifi::init () {
    Log.notice(F("[%l] [wifi] Starting ...\r\n"),  millis());
    WiFi.onEvent(cWifi::wifiEvent);
}

void cWifi::services() {
    xTaskCreate(cWifi::wifiTask,"clientTask",4096,NULL,wifiPriority,NULL); // create+connect WiFi
}