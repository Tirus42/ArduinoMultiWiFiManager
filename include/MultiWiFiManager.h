#pragma once

#include <functional>
#include <string>
#include <Print.h>

class MultiWiFiManager {
	private:
		struct WiFiEntry {
			std::string ssid;
			std::string password;

			WiFiEntry(const std::string& ssid, const std::string& password);
		};

		std::vector<WiFiEntry> networks;
		uint16_t connectionIndex;
		bool connected:1;
		uint32_t connectStartTime;

		std::function<void()> optOnConnectedCallback;
		std::function<void()> optOnDisconnectedCallback;
		Print* optLogTarget;

		void tryNetwork(uint16_t index, uint32_t currentTime);

	public:
		MultiWiFiManager(Print* optLogTarget = nullptr);

		/// Adds a (additional) WiFi to the list
		MultiWiFiManager* addWiFi(const char* ssid, const char* password);

		MultiWiFiManager* setOnConnectedCallback(const std::function<void()>& callback);
		MultiWiFiManager* setOnDisconnectedCallback(const std::function<void()>& callback);

		// Update function, must be called in the main loop
		void update();
		void update(uint32_t currentTime);
};
