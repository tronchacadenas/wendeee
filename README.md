# wendeee

Wendeee is a Smart Fan Speed control unit.

### Features ###

It controls the speed of a Fan with the Heart Rate

- Web-based configuration and control UI
- Bluetooth LE Heart Rate Monitor Client
- Bluetooth LE Rebroacast Server

### W A R N I N G ###

**Device comes without warranty**

**Never leave device unattended**

**Do NOT exceed 160W AC power**

**Always unplug after use**

### Hardware ###

- ESP32
- Thrysistor AC Speed controller
- Bluetooth Heart Rate Monitor

### Howto use ###

- Connect to power (AC 240V)
- Configure
    * Open address 192.168.4.1 (or wendeee.local) in phone/tablet/PC
    * Set control values for FAN and HRM
    * *[optional]* Set Fan mode auto or manual
    * *[optional]* Set Bluetooth HRM band to auto connect or on-the-fly
    * *[optional]* Set WiFi credential and connect to existing WiFi network

### Speed Control features  ###

Fan speed is controlled with Bluetooth LE connected Heart Rate (or manually):

- *Fan speed* is linearly controlled by *heart rate*
- Below *min heart rate* speed is set to *min Fan*
- Above *max heart rate* speed is set to *max Fan*