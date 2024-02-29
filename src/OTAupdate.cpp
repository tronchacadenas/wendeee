#include "OTAupdate.h"

// https://github.com/s00500/ESPUI/issues/116

/*--------------------------------------------------------------------------------*/
/*      O T A    U P D A T E                                                      */
/*--------------------------------------------------------------------------------*/

const char* OTA_INDEX PROGMEM
    = R"=====(<!DOCTYPE html><html><head><meta charset=utf-8><title>OTA</title></head><body><div class="upload"><form method="POST" action="/ota" enctype="multipart/form-data"><input type="file" name="data" /><input type="submit" name="upload" value="Upload" title="Upload Files"></form></div></body></html>)=====";

void cOTAupdate::update(AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final)
{
    if (!index)
    {
        Log.notice(F("[%l] [ota] Updating ...\r\n"),  millis());
        Log.notice(F("[%l] [ota] filename: %s ...\r\n"),  millis(), filename.c_str());
         // calculate sketch space required for the update, for ESP32 use the max constant
        if (!Update.begin(UPDATE_SIZE_UNKNOWN))
        {
            // start with max available size
            Update.printError(Serial);
        }
    }

    if (len)
    {
        Update.write(data, len);
    }

    // if the final flag is set then this is the last frame of data
    if (final)
    {
        if (Update.end(true))
        {
            // true to set the size to the current progress
            Log.notice(F("[%l] [ota] Update Success: %ub written\r\n"),  millis(), index + len);
            Log.notice(F("[%l] [ota] Rebooting ...\r\n"),  millis());
            ESP.restart();
        }
        else
        {
            Update.printError(Serial);
        }
    }
}

void cOTAupdate::initTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        // IMPORTANT !!
        // wait until ESPUI is ready
        while (ESPUIready == false)  {
            vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait 1000ms
        }

        Log.notice(F("[%l] [ota] Starting OTA server ...\r\n"),  millis());

        // init OTA
        ESPUI.server->on("/ota", 
            HTTP_POST, 
            [](AsyncWebServerRequest* request) { request->send(200); }, 
            cOTAupdate::update);

        ESPUI.server->on("/ota", 
            HTTP_GET, 
            [](AsyncWebServerRequest* request) {
                AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", OTA_INDEX);
                request->send(response); });

        vTaskDelete( NULL ); // end task

    }
}

void cOTAupdate::init() {
    xTaskCreate(cOTAupdate::initTask, "initTask", 4096,NULL,webPriority,NULL);
}