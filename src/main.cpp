#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "wsEventHandler.h"
#include "gpio.h"

#define SSID "badge-controller.com" // This is the SSID that ESP32 will broadcast
#define CAPTIVE_DOMAIN "http://badge-controller.com" // This is the SSID that ESP32 will broadcast
// Uncomment the following line to enable password in the wifi access point
#define PASSWORD "19091909" // password should be at least 8 characters to make it work
#define DNS_PORT 53
// Options to enable serial printing
#define VERBOSE

const IPAddress apIP(192, 168, 2, 1);
const IPAddress gateway(255, 255, 255, 0);

DNSServer dnsServer;
AsyncWebServer server(80);
AsyncWebSocket websocket("/ws");

void redirectToIndex(AsyncWebServerRequest *request) {
#ifdef CAPTIVE_DOMAIN
    request->redirect(CAPTIVE_DOMAIN);
#else
  request->redirect("http://" + apIP.toString());
#endif
}

void setup() {
    setupGpio();
#ifdef VERBOSE
    Serial.begin(115200);
#endif

    WiFi.disconnect(); // added to start with the Wi-Fi off, avoid crashing
    WiFiClass::mode(WIFI_OFF); // added to start with the Wi-Fi off, avoid crashing
    WiFiClass::mode(WIFI_AP);
#ifndef PASSWORD
  WiFi.softAP(SSID);
#else
    WiFi.softAP(SSID, PASSWORD);
#endif
    WiFi.softAPConfig(apIP, apIP, gateway);
    dnsServer.start(DNS_PORT, "*", apIP);

#ifdef VERBOSE
    Serial.print("\nWiFi AP: ");
    Serial.print(SSID);
    Serial.print("\nWiFi Password: ");
    Serial.print(PASSWORD);
    Serial.print("\nIP Address: ");
    Serial.println(WiFi.softAPIP());
#endif

    if (!LittleFS.begin()) {
#ifdef VERBOSE
        Serial.println("An Error has occurred while mounting LittleFS");
#endif
        return;
    }

    // bind websocket to async web server
    websocket.onEvent(wsEventHandler);
    server.addHandler(&websocket);
    // setup static web server
    server.serveStatic("/", LittleFS, "/www/")
            .setDefaultFile("index.html");
    // Captive portal to keep the client
    server.onNotFound(redirectToIndex);
    server.begin();

#ifdef VERBOSE
    Serial.println("Server Started");
#endif
}

void loop() {
    // serve DNS request for captive portal
    dnsServer.processNextRequest();
    vTaskDelay(1);
}
