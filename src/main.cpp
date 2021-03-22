/**
 * @file main.cpp
 * @author Elvandry Ghiffary (elvandry13@gmail.com)
 * @brief Blink LED with OTA feature
 * @version 0.1
 * @date 2021-03-22
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClient.h>
#include "config.h"

#define LED_PIN 5

String FWVersion = "1.0.5";

// Create config.h that contains variables below
const char *fwUrlBase = FW_URL_BASE; // http://your-server:port/
const char *ssid = SSID;
const char *password = PSWD;

unsigned long timeout = 0;

WiFiClient client;	

void checkForUpdates()
{
	String fwURL = String(fwUrlBase);
	fwURL.concat("firmware");

	String fwVersionURL = fwURL;
	fwVersionURL.concat(".version");
	Serial.println("Checking for firmware updates.");
	Serial.print("Firmware version URL : ");
	Serial.println(fwVersionURL);

	HTTPClient httpClient;
	httpClient.begin(client, fwVersionURL);
	int httpCode = httpClient.GET();
	if (httpCode == 200)
	{
		String newFWVersion = httpClient.getString();
		Serial.print("Current firmware version : ");
		Serial.println(FWVersion);
		Serial.print("Available firmware version : ");
		Serial.println(newFWVersion);
		if (newFWVersion != FWVersion)
		{
			String fwImageURL = fwURL;
			fwImageURL.concat(".bin");
			Serial.println("Updating...");
			t_httpUpdate_return ret = httpUpdate.update(client, fwImageURL);
			switch (ret)
			{
			case HTTP_UPDATE_FAILED:
				Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
				break;

			case HTTP_UPDATE_NO_UPDATES:
				Serial.println("HTTP_UPDATE_NO_UPDATES");
				break;
			case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                break;
			}
		}
		else
		{
			Serial.println("Already on latest version");
		}
	}
	else
	{
		Serial.print("Firmware version check failed, got HTTP response code ");
		Serial.println(httpCode);
	}
	httpClient.end();
}

void setup()
{
	Serial.begin(115200);

	WiFi.mode(WIFI_STA);
	Serial.print("Connecting to ");
	Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address : ");
	Serial.println(WiFi.localIP());
	Serial.print("Firmware Version : ");
	Serial.println(FWVersion);

	// LED pin setup
	pinMode(LED_PIN, OUTPUT);
	digitalWrite(LED_PIN, HIGH);
}

void loop()
{
	// Blink
	digitalWrite(LED_PIN, LOW);
	delay(200);
	digitalWrite(LED_PIN, HIGH);
	delay(200);

	// Check updates every 30 s
	if (millis() - timeout > 30000)
	{
		checkForUpdates();
		timeout = millis();
	}
}
