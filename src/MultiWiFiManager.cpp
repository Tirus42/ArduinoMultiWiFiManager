#include "MultiWiFiManager.h"

#include "WiFi.h"

static constexpr uint32_t PER_CONNECTION_TIME_MS = 15000;
static constexpr uint16_t INVALID_INDEX = std::numeric_limits<uint16_t>::max();

MultiWiFiManager::WiFiEntry::WiFiEntry(const std::string& ssid, const std::string& password) :
    ssid(ssid),
    password(password) {}

MultiWiFiManager::MultiWiFiManager(Print* optLogTarget) :
    networks(),
    connectionIndex(INVALID_INDEX),
    connected(false),
    connectStartTime(),
    optOnConnectedCallback(),
    optOnDisconnectedCallback(),
    optLogTarget(optLogTarget) {}

MultiWiFiManager* MultiWiFiManager::addWiFi(const char* ssid, const char* password) {
    networks.push_back(WiFiEntry(ssid, password));
    return this;
}

MultiWiFiManager* MultiWiFiManager::setOnConnectedCallback(const std::function<void()>& callback) {
    optOnConnectedCallback = callback;
    return this;
}

void MultiWiFiManager::tryNetwork(uint16_t index, uint32_t currentTime) {
    WiFi.begin(networks[index].ssid.c_str(), networks[index].password.c_str());

    connectionIndex = index;
    connectStartTime = currentTime;

    if (optLogTarget)
        optLogTarget->printf("Try to connect to WiFi %u with SSID '%s' ...\n", index, networks[index].ssid.c_str());
}

void MultiWiFiManager::update() {
    update(millis());
}

void MultiWiFiManager::update(uint32_t currentTime) {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        if (!connected) {
            if (optLogTarget)
                optLogTarget->printf("Connected to SSID '%s' after %u ms\n", networks[connectionIndex].ssid.c_str(), currentTime - connectStartTime);

            connected = true;

            if (optOnConnectedCallback) {
                if (optLogTarget)
                    optLogTarget->printf("Executing onConnectedCallback ...\n");

                optOnConnectedCallback();
            }
        }

        return;
    } else if (status == WL_DISCONNECTED && connected) {
        connected = false;

        if (optLogTarget)
            optLogTarget->printf("Connection to SSID '%s' lost\n", networks[connectionIndex].ssid.c_str());

		if (optOnDisconnectedCallback)
			optOnDisconnectedCallback();
    }

    if (connectionIndex == networks.size())
        return;	// All networks tried, abort here

    if (connectionIndex == INVALID_INDEX) {
        tryNetwork(0, currentTime);
        return;
    }

    if (connectStartTime + PER_CONNECTION_TIME_MS < currentTime) {
        connectionIndex++;

        if (connectionIndex < networks.size()) {
            tryNetwork(connectionIndex, currentTime);
        } else {
            WiFi.disconnect(true);

            if (optLogTarget)
                optLogTarget->printf("Unable to connect to any WiFi, disabled WiFi\n");
        }
    }
}
