#include <MultiWiFiManager.h>

/**
* Minimal example usage of the MultiWiFiManager.
* Specifies two WiFi networks to connect to.
* With the execution of update() inside the main loop
* the MultiWiFiManager will try to connect to each network
* one after another without blocking.
*/

MultiWiFiManager multiWifiManager;

void setup() {
    multiWifiManager.addWiFi("MyWiFiWithPriority1", "<Password>");
    multiWifiManager.addWiFi("MyWiFiWithPriority2", "<Password>");
}

void loop() {
	multiWifiManager.update();
}
