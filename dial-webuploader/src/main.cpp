#include <Arduino.h>
#include <ssid.h>

#include <WebServer.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <M5Dial.h>
#include <HTTPUpdateServer.h>
const char *host = "m5dial";
const char *ssid = MY_SSID;
const char *password = MY_SSID_PASSWORD;
WebServer httpServer(80);
HTTPUpdateServer httpUpdater;

void draw_message(const char *message)
{
    M5Dial.Display.fillRect(0, M5Dial.Display.height() / 2 - 10, M5Dial.Display.width(), 20, BLACK);
    M5Dial.Display.setTextDatum(middle_center);
    M5Dial.Display.setTextFont(&fonts::Orbitron_Light_32);
    M5Dial.Display.setTextSize(0.5);
    M5Dial.Display.setTextColor(ORANGE);
    M5Dial.Display.drawString(message, M5Dial.Display.width() / 2, M5Dial.Display.height() / 2);
}

void setup()
{
    auto cfg = M5.config();
    M5Dial.begin(cfg, false, false);
    draw_message("wifi begin");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
        draw_message("connect fail");
        delay(2000);
        ESP.restart();
    }

    /*use mdns for host name resolution*/
    if (!MDNS.begin(host))
    { // http://esp32.local
        draw_message("Error MDNS !");
        while (1)
        {
            delay(1000);
        }
    }
    draw_message("MDNS start");

    httpUpdater.setup(&httpServer);
    httpServer.begin();

    MDNS.addService("http", "tcp", 80);
    draw_message(WiFi.localIP().toString().c_str());
}

void loop()
{
    httpServer.handleClient();
    delay(1);
}
