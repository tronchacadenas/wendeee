#include "WebServer.h"

/*--------------------------------------------------------------------------------*/

bool connectWifi = false;

//UI handles
uint16_t tabInfo;
uint16_t tabControl;
uint16_t tabBluetooth;
uint16_t tabWifi;
uint16_t tabReset;
uint16_t Info_InfoGroup;
uint16_t Info_HRM;
uint16_t Info_FAN;
uint16_t Info_BLE;
uint16_t Info_WIFI;
uint16_t Control_Fan;
uint16_t Control_Mode;
uint16_t Control_Auto;
uint16_t Control_FanMinSlider;
uint16_t Control_FanMaxSlider;
uint16_t Control_HrmMinSlider;
uint16_t Control_HrmMaxSlider;
uint16_t Bluetooth_InfoGroup;
uint16_t Bluetooth_HRM;
uint16_t Bluetooth_BLEClient;
uint16_t Bluetooth_BLEServer;
uint16_t Bluetooth_BLEName;
uint16_t Bluetooth_DeviceGroup;
uint16_t Bluetooth_DeviceName;
uint16_t Bluetooth_DeviceMac;
uint16_t Bluetooth_Disconnect;
uint16_t Bluetooth_Bond;
uint16_t Wifi_InfoGroup;
uint16_t Wifi_Client;
uint16_t Wifi_ClientIP;
uint16_t Wifi_Server;
uint16_t Wifi_ServerAP;
uint16_t Wifi_ServerIP;
uint16_t Wifi_Hostname;
uint16_t Wifi_CredentialGroup;
uint16_t Wifi_SSID;
uint16_t Wifi_Password;
uint16_t Wifi_Connect;
uint16_t Wifi_Reboot;
uint16_t Wifi_Auto;
uint16_t Reset_RebootGroup;
uint16_t Reboot_Query;
uint16_t Reboot_Reboot;
uint16_t Reset_ResetGroup;
uint16_t Reset_Query;
uint16_t Reset_Reboot;
uint16_t Reset_Yes;
uint16_t Reset_No;
uint16_t Reset_InfoGroup;
uint16_t Info_Version;

// CSS STYLES
    
String HEADER               = "display: none;";
String INFO_TEXT            = "background-color: unset; width: 35%;  text-align: right; padding: 0% 4% 0% 4%; vertical-align: baseline;";
String INFO_TEXT_SWITCH     = "background-color: unset; width: 35%;  text-align: right; padding: 0% 4% 0% 4%; vertical-align: middle;";
String INFO_DATASHORT_UNITS = "width: 45%; text-align: center;";
String INFO_DATASHORT_GREY  = "width: 45%; text-align: center; margin-right: 20%;";
String INFO_DATASHORT_GREEN = "background-color: #76d21d; width: 45%; text-align: center; margin-right: 20%;";
String INFO_DATASHORT_RED   = "background-color: #d37c7c; width: 45%; text-align: center; margin-right: 20%;";
String INFO_DATALONG        = "width: 60%; text-align: center;";
String INFO_UNITS           = "background-color: unset; width: 20%;  text-align: left; padding: 0% 4% 0% 4%; vertical-align: baseline;";
String INFO_SWITCH          = "width: 60px; margin-right: calc(60% - 60px); vertical-align: baseline";
String SLIDER               = "width: 65%; vertical-align: middle; height: 25px; margin-left: 20%;";
String SLIDER_TEXT          = "background-color: unset; width: 23%; text-align: right; vertical-align: top; margin-top: -23px; margin-left: -4%; margin-right: 82%";
String BUTTON               = "width: 40%; margin-left: 40%;";
String BUTTON_CONNECT       = "width: 40%; margin-left: 40%; background-color: #76d21d;";
String BUTTON_QUERY         = "width: 55%; margin-left: 0%; background-color: #d37c7c;";
String BUTTON_REBOOT        = "width: 55%; margin-left: 0%; background-color: #76d21d;";
String BUTTON_YES           = "width: 20%; margin-left: 0%; background-color: #d37c7c;";
String BUTTON_NO            = "width: 20%; margin-left: 10%;";
String BUTTON_HIDDEN        = "width: 20%; margin-left: 0%; display: none;";

/*--------------------------------------------------------------------------------*/
/*      W E B    S E R V E R                                                      */
/*--------------------------------------------------------------------------------*/

void cWebServer::rebootCall(Control* sender, int eventname) {
    if (eventname == B_DOWN)
    {
        // show rebooting banner
        ESPUI.setElementStyle(Reboot_Query, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Reboot_Reboot, BUTTON_REBOOT);
    }
    if (eventname == B_UP)
    {
        Log.notice(F("[%l] [web] Reboot ... \r\n"),  millis());
        ESP.restart();
    }
}

void cWebServer::resetCall(Control* sender, int eventname) {
    if (eventname == B_UP)
    {
        Log.verbose(F("[%l] [web] reset pressed\r\n"),  millis());
        ESPUI.setElementStyle(Reset_Query, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Reset_Yes, BUTTON_YES);
        ESPUI.setElementStyle(Reset_No, BUTTON_NO);
    }
}

void cWebServer::yesCall(Control* sender, int eventname) {
    if (eventname == B_DOWN)
    {
        Log.verbose(F("[%l] [web] yes pressed\r\n"),  millis());
        
        // reset all eeprom configs
        Log.notice(F("[%l] [web] Reset saved data\r\n"),  millis());
        oStorage.reset();

        // show rebooting banner
        ESPUI.setElementStyle(Reset_Query, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Reset_Yes, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Reset_No, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Reset_Reboot, BUTTON_REBOOT);
    }
    if (eventname == B_UP)
    {   
        // reboot
        Log.notice(F("[%l] [web] Reboot ... \r\n"),  millis());
        ESP.restart();
    }
}

void cWebServer::noCall(Control* sender, int eventname) {
    if (eventname == B_UP)
    {
        Log.verbose(F("[%l] [web] no pressed\r\n"),  millis());
        // redo interface
        ESPUI.setElementStyle(Reset_Query, BUTTON_QUERY);
        ESPUI.setElementStyle(Reset_Yes, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Reset_No, BUTTON_HIDDEN);
    }
}

void cWebServer::clientBondCall(Control* sender, int eventname) {
    if (eventname == S_ACTIVE)
    {
        clientBond = true;
        Log.verbose(F("[%l] [web] clientBond ON\r\n"),  millis());

        // save to memory
        Log.notice(F("[%l] [web] Saving data\r\n"),  millis());
        oStorage.ble();

    }
    else if (eventname == S_INACTIVE)
    {
        clientBond = false;
        Log.verbose(F("[%l] [web] clientBond OFF\r\n"),  millis());

        // save to memory
        Log.notice(F("[%l] [web] Saving data\r\n"),  millis());
        oStorage.ble();
    }
}

void cWebServer::disconnectCall(Control* sender, int eventname) {
    
    if (clientMode != MODE_NOTIFY) return; // hack to prevent phone false button clicks on phones
    
    if (eventname == B_UP)
    {
        // remove ble info
        clientName="";
        clientAddress="";
        
        clientMode = MODE_DISCONNECT;
        Log.verbose(F("[%l] [web] disconnect client\r\n"),  millis());

        // disable clientBond
        clientBond = false;
        ESPUI.updateSwitcher(Bluetooth_Bond,false);
        Log.trace(F("[%l] [web] clientBond: %d\r\n"),  millis(), clientBond);

        // save (erase) data
        Log.notice(F("[%l] [web] Erasing data\r\n"),  millis());
        oStorage.ble();
    }
}

void cWebServer::ssidCall(Control* sender, int eventname) {
    if (eventname == T_VALUE)
    {
        wifiSSID = sender->value;
        Log.trace(F("[%l] [web] reading ssid: %s\r\n"),  millis(), wifiSSID.c_str());
    }
}

void cWebServer::passwordCall(Control* sender, int eventname) {
    if (eventname == T_VALUE)
    {
        wifiPassword = sender->value;
        
        //  hide password
        ESPUI.updateText(Wifi_Password,"**hidden**");

        // enable connect button
        ESPUI.setEnabled(Wifi_Connect, true);
        connectWifi = true;

        // Wifi_Password
        Log.trace(F("[%l] [web] reading password: %s\r\n"),  millis(), wifiPassword.c_str());
    }
}

void cWebServer::connectCall(Control* sender, int eventname) {

    if (connectWifi == false) return; // hack to prevent phone false button clicks on phones

    if (eventname == B_DOWN)
    {
        // activate auto connect
        wifiAuto = true;
        ESPUI.updateSwitcher(Wifi_Auto, true);

        Log.trace(F("[%l] [web] reading password: %s\r\n"),  millis(), ESPUI.getControl(Wifi_Password)->value.c_str());

        // save values to memory
        Log.notice(F("[%l] [web] Saving data\r\n"),  millis());
        oStorage.wifi();

        // show rebooting banner
        ESPUI.setElementStyle(Wifi_Connect, BUTTON_HIDDEN);
        ESPUI.setElementStyle(Wifi_Reboot, BUTTON_CONNECT);
    }

    if (eventname == B_UP)
    {      
        // reboot
        Log.notice(F("[%l] [web] Reboot ... \r\n"),  millis());
        ESP.restart();
    }
}

void cWebServer::wifiAutoCall(Control* sender, int eventname) {
    if (eventname == S_ACTIVE)
    {
        wifiAuto = true;
        Log.verbose(F("[%l] [web] wifiAuto ON\r\n"),  millis());
    }
    else if (eventname == S_INACTIVE)
    {
        wifiAuto = false;
        Log.verbose(F("[%l] [web] wifiAuto OFF\r\n"),  millis());
    }
}

void cWebServer::fanAutoCall(Control* sender, int eventname) {
    if (eventname == S_ACTIVE)
    {
        fanAuto = true;
        Log.verbose(F("[%l] [web] fanAuto ON\r\n"),  millis());
    }
    else if (eventname == S_INACTIVE)
    {
        fanAuto = false;
        Log.verbose(F("[%l] [web] fanAuto OFF\r\n"),  millis());
    }
    
    ESPUI.updateLabel(Control_Mode, fanAuto ? "Auto" : "Manual"); // Manual too large for 1 line
    ESPUI.setEnabled(Control_Fan, !fanAuto); // active/read-only
    ESPUI.updateVisibility(Control_FanMinSlider, fanAuto); // hide/show
    ESPUI.updateVisibility(Control_HrmMinSlider, fanAuto); // hide/show

}

void cWebServer::fanSpeedCall(Control* sender, int eventname) {
    if (eventname == SL_VALUE && fanAuto == false)
    {
        fanSpeed = sender->value.toInt();
    }
}

void cWebServer::min_fanCall(Control* sender, int eventname) {
    if (eventname == SL_VALUE)
    {
        min_fan = sender->value.toInt(); // send value
        max_fan = max_fan < min_fan ? min_fan : max_fan; // no crossover
    }
}

void cWebServer::max_fanCall(Control* sender, int eventname) {
    if (eventname == SL_VALUE)
    {
        max_fan = sender->value.toInt(); // send value
        min_fan = min_fan > max_fan ? max_fan : min_fan; // no crossover
    }
}

void cWebServer::min_hrmCall(Control* sender, int eventname) {
    if (eventname == SL_VALUE)
    {
        min_hrm = sender->value.toInt(); // send value
        max_hrm = max_hrm < min_hrm ? min_hrm : max_hrm; // no crossover
    }
}

void cWebServer::max_hrmCall(Control* sender, int eventname) {
    if (eventname == SL_VALUE)
    {
        max_hrm = sender->value.toInt(); // send value
        min_hrm = min_hrm > max_hrm ? max_hrm : min_hrm; // no crossover
    }
}

void cWebServer::webInterface() {
   
    //Make sliders continually report their position as they are being dragged.
    ESPUI.sliderContinuous = true;

    ///////// TABS /////////

    tabInfo         = ESPUI.addControl(Tab, " ", "Info");
    tabControl      = ESPUI.addControl(Tab, " ", "Control");
    tabBluetooth    = ESPUI.addControl(Tab, " ", "BLE");
    tabWifi         = ESPUI.addControl(Tab, " ", "WiFi");
    tabReset        = ESPUI.addControl(Tab, " ", "!");

    ///////// INFO /////////

    /////////       STATUS /////////

    Info_InfoGroup  = ESPUI.addControl(Label,  "Status", "", Dark, tabInfo);
                      ESPUI.setElementStyle(Info_InfoGroup, HEADER);

                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","HRM", None, Info_InfoGroup), INFO_TEXT);
    Info_HRM        = ESPUI.addControl(Label, " ", " ", Dark, Info_InfoGroup, NULL);
                      ESPUI.setElementStyle(Info_HRM, INFO_DATASHORT_UNITS);
                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","bpm", None, Info_InfoGroup), INFO_UNITS);

                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","FAN", None, Info_InfoGroup), INFO_TEXT);
    Info_FAN        = ESPUI.addControl(Label, " ", " ", Dark, Info_InfoGroup, NULL);
                      ESPUI.setElementStyle(Info_FAN, INFO_DATASHORT_UNITS);
                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","%", None, Info_InfoGroup), INFO_UNITS);

                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","BLE", None, Info_InfoGroup), INFO_TEXT);
    Info_BLE        = ESPUI.addControl(Label, " ", "OK", Dark, Info_InfoGroup, NULL);
                      ESPUI.setElementStyle(Info_BLE, INFO_DATASHORT_GREEN);

                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","WIFI", None, Info_InfoGroup), INFO_TEXT);
    Info_WIFI       = ESPUI.addControl(Label, " ", "OK", Dark, Info_InfoGroup, NULL);
                      ESPUI.setElementStyle(Info_WIFI, INFO_DATASHORT_GREEN);

    ///////// CONTROL /////////

    /////////       FAN SPEED /////////

                    //   ESPUI.addControl(Separator, "Mode", " ", None, tabControl);

    Control_Fan     = ESPUI.addControl(Slider, "Fan Speed", "30", Dark, tabControl, &fanSpeedCall);
                      ESPUI.setElementStyle(Control_Fan, SLIDER);

    Control_Mode    = ESPUI.addControl(Label," ","Auto", None, Control_Fan);
                      ESPUI.setElementStyle(Control_Mode, SLIDER_TEXT);

                      ESPUI.setElementStyle(ESPUI.addControl(Label," ","Auto Control", None, Control_Fan), INFO_TEXT_SWITCH);
    Control_Auto    = ESPUI.addControl(Switcher, " ", String(fanAuto), Dark, Control_Fan, &fanAutoCall);
                      ESPUI.setElementStyle(Control_Auto, INFO_SWITCH);

    /////////       LIMITS /////////

                           ESPUI.addControl(Separator, "Limits", " ", None, tabControl);

    Control_FanMinSlider = ESPUI.addControl(Slider, "Fan", String(min_fan), Dark, tabControl, &min_fanCall);
                           ESPUI.setElementStyle(Control_FanMinSlider, SLIDER);
                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","min", None, Control_FanMinSlider), SLIDER_TEXT);
    
    Control_FanMaxSlider = ESPUI.addControl(Slider, " ", String(max_fan), Dark, Control_FanMinSlider, &max_fanCall);
                           ESPUI.setElementStyle(Control_FanMaxSlider, SLIDER);
                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","max", None, Control_FanMinSlider), SLIDER_TEXT);

    Control_HrmMinSlider = ESPUI.addControl(Slider, "Heart Rate", String(min_hrm), Dark, tabControl, &min_hrmCall);
                           ESPUI.setElementStyle(Control_HrmMinSlider, SLIDER);
                           ESPUI.addControl(Max, " ",   "200", None, Control_HrmMinSlider);
                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","min", None, Control_HrmMinSlider), SLIDER_TEXT);
    
    Control_HrmMaxSlider = ESPUI.addControl(Slider, " ", String(max_hrm), Dark, Control_HrmMinSlider, &max_hrmCall);
                           ESPUI.setElementStyle(Control_HrmMaxSlider, SLIDER);
                           ESPUI.addControl(Max, " ",   "200", None, Control_HrmMaxSlider);
                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","max", None, Control_HrmMinSlider), SLIDER_TEXT);

    ///////// BLUETOOTH /////////

    /////////       STATUS /////////

    Bluetooth_InfoGroup  = ESPUI.addControl(Label,  "Status", "", Dark, tabBluetooth);
                           ESPUI.setElementStyle(Bluetooth_InfoGroup, HEADER);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","HRM", None, Bluetooth_InfoGroup), INFO_TEXT);
    Bluetooth_HRM        = ESPUI.addControl(Label, " ", " ", Dark, Bluetooth_InfoGroup);
                           ESPUI.setElementStyle(Bluetooth_HRM, INFO_DATASHORT_UNITS);
                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","bpm", None, Bluetooth_InfoGroup), INFO_UNITS);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","BLE Client", None, Bluetooth_InfoGroup), INFO_TEXT);
    Bluetooth_BLEClient  = ESPUI.addControl(Label, " ", "Waiting", Dark, Bluetooth_InfoGroup);
                           ESPUI.setElementStyle(Bluetooth_BLEClient, INFO_DATASHORT_RED);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","BLE Server", None, Bluetooth_InfoGroup), INFO_TEXT);
    Bluetooth_BLEServer  = ESPUI.addControl(Label, " ", "Waiting", Dark, Bluetooth_InfoGroup);
                           ESPUI.setElementStyle(Bluetooth_BLEServer, INFO_DATASHORT_GREY);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","Name", None, Bluetooth_InfoGroup), INFO_TEXT);
    Bluetooth_BLEName    = ESPUI.addControl(Label, " ", " ", Dark, Bluetooth_InfoGroup);
                           ESPUI.setElementStyle(Bluetooth_BLEName, INFO_DATASHORT_GREY);

    /////////       HEART RATE MONITOR /////////

                            ESPUI.addControl(Separator, "Heart Rate Monitor",         " ", None, tabBluetooth);

    Bluetooth_DeviceGroup = ESPUI.addControl(Label,  "Device", "", Dark, tabBluetooth);
                            ESPUI.setElementStyle(Bluetooth_DeviceGroup, HEADER);

                            ESPUI.setElementStyle(ESPUI.addControl(Label," ","Name", None, Bluetooth_DeviceGroup), INFO_TEXT);
    Bluetooth_DeviceName  = ESPUI.addControl(Text, " ", "Heart rate band", Dark, Bluetooth_DeviceGroup);
                            ESPUI.setElementStyle(Bluetooth_DeviceName, INFO_DATALONG);
                            ESPUI.setEnabled(Bluetooth_DeviceName, false); // read only

                            ESPUI.setElementStyle(ESPUI.addControl(Label," ","MAC", None, Bluetooth_DeviceGroup), INFO_TEXT);
    Bluetooth_DeviceMac   = ESPUI.addControl(Text, " ", "XX:XX:XX:XX:XX:XX", Dark, Bluetooth_DeviceGroup);
                            ESPUI.setElementStyle(Bluetooth_DeviceMac, INFO_DATALONG);
                            ESPUI.setEnabled(Bluetooth_DeviceMac, false); // read only

    Bluetooth_Disconnect  = ESPUI.addControl(Button, " ", "Disconnect", Dark, Bluetooth_DeviceGroup, &disconnectCall);
                            ESPUI.setElementStyle(Bluetooth_Disconnect, BUTTON);

                            ESPUI.setElementStyle(ESPUI.addControl(Label," ","Auto Connect", None, Bluetooth_DeviceGroup), INFO_TEXT_SWITCH);
    Bluetooth_Bond        = ESPUI.addControl(Switcher, " ", String(clientBond), Dark, Bluetooth_DeviceGroup, &clientBondCall);
                            ESPUI.setElementStyle(Bluetooth_Bond, INFO_SWITCH);

    
    ///////// WIFI /////////

    /////////       STATUS /////////

    Wifi_InfoGroup = ESPUI.addControl(Label,  "Status", "", Dark, tabWifi);
                     ESPUI.setElementStyle(Wifi_InfoGroup, HEADER);

                    ESPUI.setElementStyle(ESPUI.addControl(Label," ","Client", None, Wifi_InfoGroup), INFO_TEXT);
    Wifi_Client   = ESPUI.addControl(Label, " ", "OFF", Dark, Wifi_InfoGroup);
                    ESPUI.setElementStyle(Wifi_Client, INFO_DATASHORT_GREY);

                    ESPUI.setElementStyle(ESPUI.addControl(Label," ","IP", None, Wifi_InfoGroup), INFO_TEXT);
    Wifi_ClientIP = ESPUI.addControl(Label, " ", " ", Dark, Wifi_InfoGroup);
                    ESPUI.setElementStyle(Wifi_ClientIP, INFO_DATASHORT_GREY);

                    ESPUI.setElementStyle(ESPUI.addControl(Label," ","Server", None, Wifi_InfoGroup), INFO_TEXT);
    Wifi_Server   = ESPUI.addControl(Label, " ", "OFF", Dark, Wifi_InfoGroup);
                    ESPUI.setElementStyle(Wifi_Server, INFO_DATASHORT_GREY);

                    ESPUI.setElementStyle(ESPUI.addControl(Label," ","SSID", None, Wifi_InfoGroup), INFO_TEXT);
    Wifi_ServerAP = ESPUI.addControl(Label, " ", " ", Dark, Wifi_InfoGroup);
                    ESPUI.setElementStyle(Wifi_ServerAP, INFO_DATASHORT_GREY);

                    ESPUI.setElementStyle(ESPUI.addControl(Label," ","IP", None, Wifi_InfoGroup), INFO_TEXT);
    Wifi_ServerIP = ESPUI.addControl(Label, " ", " ", Dark, Wifi_InfoGroup);
                    ESPUI.setElementStyle(Wifi_ServerIP, INFO_DATASHORT_GREY);

                    ESPUI.setElementStyle(ESPUI.addControl(Label," ","Hostname", None, Wifi_InfoGroup), INFO_TEXT);
    Wifi_Hostname = ESPUI.addControl(Label, " ", hostname + ".local", Dark, Wifi_InfoGroup);
                    ESPUI.setElementStyle(Wifi_Hostname, INFO_DATASHORT_GREY);

    /////////       CREDENTIALS /////////

                           ESPUI.addControl(Separator, "Credentials",         " ", None, tabWifi);

    Wifi_CredentialGroup = ESPUI.addControl(Label,  "Network", "", Dark, tabWifi);
                           ESPUI.setElementStyle(Wifi_CredentialGroup, HEADER);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","SSID", None, Wifi_CredentialGroup), INFO_TEXT);
    Wifi_SSID            = ESPUI.addControl(Text, " ", wifiSSID, Dark, Wifi_CredentialGroup, &ssidCall);
                           ESPUI.setElementStyle(Wifi_SSID, INFO_DATALONG);
                           ESPUI.addControl(Max, " ", "32", None, Wifi_SSID);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","Password", None, Wifi_CredentialGroup), INFO_TEXT);
    Wifi_Password        = ESPUI.addControl(Text, " ",  "**hidden**", Dark, Wifi_CredentialGroup, &passwordCall);
                           ESPUI.setElementStyle(Wifi_Password, INFO_DATALONG);
                           ESPUI.addControl(Max, " ", "64", None, Wifi_Password);

    Wifi_Connect         = ESPUI.addControl(Button, " ", "Connect", Dark, Wifi_CredentialGroup, &connectCall);
                           ESPUI.setElementStyle(Wifi_Connect, BUTTON);
                           ESPUI.setEnabled(Wifi_Connect, false); //disable until password updated

    Wifi_Reboot          = ESPUI.addControl(Button, " ", "Rebooting ...", Dark, Wifi_CredentialGroup, &connectCall);
                           ESPUI.setElementStyle(Wifi_Reboot, BUTTON_HIDDEN);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","Auto Connect", None, Wifi_CredentialGroup), INFO_TEXT_SWITCH);
    Wifi_Auto            = ESPUI.addControl(Switcher, " ", String(wifiAuto), Dark, Wifi_CredentialGroup, &wifiAutoCall);
                           ESPUI.setElementStyle(Wifi_Auto, INFO_SWITCH);

    ///////// RESET /////////

    Reset_RebootGroup    = ESPUI.addControl(Label,  "Reboot", "", Dark, tabReset);
                           ESPUI.setElementStyle(Reset_RebootGroup, HEADER);

    Reboot_Query         = ESPUI.addControl(Button, " ", "Reboot", Dark, Reset_RebootGroup, &rebootCall);
                           ESPUI.setElementStyle(Reboot_Query, BUTTON_QUERY);

    Reboot_Reboot        = ESPUI.addControl(Button, " ", "Rebooting ...", Dark, Reset_RebootGroup, &rebootCall);
                           ESPUI.setElementStyle(Reboot_Reboot, BUTTON_HIDDEN);


    Reset_ResetGroup     = ESPUI.addControl(Label,  "Reset", "", Dark, tabReset);
                           ESPUI.setElementStyle(Reset_ResetGroup, HEADER);

    Reset_Query          = ESPUI.addControl(Button, " ", "Reset", Dark, Reset_ResetGroup, &resetCall);
                           ESPUI.setElementStyle(Reset_Query, BUTTON_QUERY);

    Reset_Reboot         = ESPUI.addControl(Button, " ", "Rebooting ...", Dark, Reset_ResetGroup, &yesCall);
                           ESPUI.setElementStyle(Reset_Reboot, BUTTON_HIDDEN);

    Reset_Yes            = ESPUI.addControl(Button, " ", "YES", Dark, Reset_ResetGroup, &yesCall);
                           ESPUI.setElementStyle(Reset_Yes, BUTTON_HIDDEN);

    Reset_No             = ESPUI.addControl(Button, " ", "NO", Dark, Reset_ResetGroup, &noCall);
                           ESPUI.setElementStyle(Reset_No, BUTTON_HIDDEN);


    Reset_InfoGroup      = ESPUI.addControl(Label,  "Info", "", Dark, tabReset);
                           ESPUI.setElementStyle(Reset_ResetGroup, HEADER);

                           ESPUI.setElementStyle(ESPUI.addControl(Label," ","Version", None, Reset_InfoGroup), INFO_TEXT);
    Info_Version         = ESPUI.addControl(Label, " ", VERSION_SW, Dark, Reset_InfoGroup);
                           ESPUI.setElementStyle(Info_Version, INFO_DATASHORT_GREY);

}

void cWebServer::webUpdate() {

    ///////// INFO /////////

    /////////       STATUS /////////

    ESPUI.updateLabel(Info_HRM, String(hrmData[1]));
    ESPUI.updateLabel(Info_FAN, String(fanSpeed));

    ESPUI.updateLabel(Info_BLE,     clientMode == MODE_NOTIFY ? "OK"                 : "WAITING");
    ESPUI.setElementStyle(Info_BLE, clientMode == MODE_NOTIFY ? INFO_DATASHORT_GREEN : INFO_DATASHORT_RED);

    ESPUI.updateLabel(Info_WIFI,     (wifiClientReady && wifiServerReady) ? "OK"                 : "WAITING");
    ESPUI.setElementStyle(Info_WIFI, (wifiClientReady && wifiServerReady) ? INFO_DATASHORT_GREEN : INFO_DATASHORT_RED);

    ///////// CONTROL /////////

    /////////       FAN SPEED /////////

    ESPUI.setEnabled(Control_Fan, !fanAuto);     // active/read-only
    ESPUI.updateSlider(Control_Fan, fanSpeed);   // read value

    /////////       LIMITS /////////

    ESPUI.updateVisibility(Control_FanMinSlider, fanAuto);  // hide/show
    ESPUI.updateVisibility(Control_HrmMinSlider, fanAuto);  // hide/show

    ESPUI.updateSlider(Control_FanMinSlider, min_fan);
    ESPUI.updateSlider(Control_FanMaxSlider, max_fan);
    ESPUI.updateSlider(Control_HrmMinSlider, min_hrm);
    ESPUI.updateSlider(Control_HrmMaxSlider, max_hrm);

    ///////// BLUETOOTH /////////

    /////////       STATUS /////////

    ESPUI.updateLabel(Bluetooth_HRM, String(hrmData[1]));

    ESPUI.updateLabel(Bluetooth_BLEClient,     clientMode == MODE_NOTIFY ? "CONNECTED"          : "WAITING");
    ESPUI.setElementStyle(Bluetooth_BLEClient, clientMode == MODE_NOTIFY ? INFO_DATASHORT_GREEN : INFO_DATASHORT_RED);

    ESPUI.updateLabel(Bluetooth_BLEServer,     (serverMode == MODE_NOTIFY && serverMode == MODE_NOTIFY) ? "CONNECTED"          : ( serverMode == MODE_OFF ? "OFF" : "WAITING"));
    ESPUI.setElementStyle(Bluetooth_BLEServer, (clientMode == MODE_NOTIFY && serverMode == MODE_NOTIFY) ? INFO_DATASHORT_GREEN : INFO_DATASHORT_GREY);
    ESPUI.updateLabel(Bluetooth_BLEName,       (serverMode == MODE_OFF) ? " " : BLEServerName);


    /////////       HEART RATE MONITOR /////////

    ESPUI.updateLabel(Bluetooth_DeviceName, clientName);
    ESPUI.updateLabel(Bluetooth_DeviceMac,  clientAddress);
    ESPUI.setEnabled(Bluetooth_Disconnect,  clientMode == MODE_NOTIFY ? true : false);

    ///////// WIFI /////////

    /////////       STATUS /////////

    ESPUI.updateLabel(Wifi_Client,     (wifiClientReady) ? "OK"                               : "WAITING");
    ESPUI.setElementStyle(Wifi_Client, (wifiClientReady) ? INFO_DATASHORT_GREEN               : INFO_DATASHORT_RED);
    ESPUI.updateLabel(Wifi_ClientIP,   (wifiClientReady) ? WiFi.localIP().toString().c_str()  : " ");

    ESPUI.updateLabel(Wifi_Server,     (wifiServerReady)  ? "OK"                               : "OFF");
    ESPUI.setElementStyle(Wifi_Server, (wifiServerReady)  ? INFO_DATASHORT_GREEN               : INFO_DATASHORT_GREY);
    ESPUI.updateLabel(Wifi_ServerAP,   (wifiServerReady)  ? WiFi.softAPSSID().c_str()          : " ");
    ESPUI.updateLabel(Wifi_ServerIP,   (wifiServerReady)  ? WiFi.softAPIP().toString().c_str() : " ");


    /////////       CREDENTIALS /////////

    // ESPUI.setEnabled(Wifi_Connect, ESPUI.getControl(Wifi_Password)->value == "**hidden**" ? false : true);

}

void cWebServer::initTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        // wait until wifi ready
        while (wifiClientReady != true && wifiServerReady != true)  {
            vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait 1000ms
        }

        Log.notice(F("[%l] [web] Starting Web Server ...\r\n"),  millis());

        //Finally, start up the UI. 
        ESPUI.begin("Wendeee Control (Beta)");

        // semaphore for OTAupdate
        ESPUIready = true;

        vTaskDelete( NULL ); // end task

    }
}

void cWebServer::webTask(void *pvParameters) {

    (void)pvParameters;
    
    while (1) { // A Task shall never return or exit.

        // wait until wifi ready
        while (wifiClientReady != true && wifiServerReady != true)  {
            vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait 1000ms
        }

        vTaskDelay( RefreshTime*1000 / portTICK_PERIOD_MS ); // wait between updates

        Log.verbose(F("[%l] [web] Updating ...\r\n"),  millis());
        cWebServer::webUpdate();
    }
}

void cWebServer::init () {

    // Create Web Interface
    Log.notice(F("[%l] [web] Creating Web Interface ...\r\n"),  millis());
    cWebServer::webInterface();

    //Turn off verbose debugging
    ESPUI.setVerbosity(Verbosity::Quiet);
    // ESPUI.setVerbosity(Verbosity::Verbose);
    // ESPUI.setVerbosity(Verbosity::VerboseJSON);

    //Start up the UI
    xTaskCreate(cWebServer::initTask,"initTask",4096,NULL,webPriority,NULL);

}

void cWebServer::update() {
    xTaskCreate(cWebServer::webTask, "webTask", 4096,NULL,webPriority,NULL);
}