#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WebServer.h>
#include <SPIFFS.h>

#include "webpage.h"

Adafruit_BME280 bme;
WebServer server(80);
unsigned long lastLog = 0;


const char* ssid = "CYBER_EXT";
const char* password = "cyberap2025";



void handleRoot()
{
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F;

String icon;
String status;

if (temperature >= 30)
{
    icon = "☀️";
    status = "Hot Weather";
}
else if (temperature >= 20)
{
    icon = "⛅";
    status = "Pleasant Weather";
}
else
{
    icon = "☁️";
    status = "Cool Weather";
}

    server.send(
        200,
        "text/html",
        getWebPage(
            temperature,
            humidity,
            pressure,
            icon,
            status
        )
    );
}

void handleData()
{
    String json = "{";
    json += "\"temperature\":" + String(bme.readTemperature(),1) + ",";
    json += "\"humidity\":" + String(bme.readHumidity(),1) + ",";
    json += "\"pressure\":" + String(bme.readPressure()/100.0,1);
    json += "}";

    server.send(200, "application/json", json);
}

void logData()
{
    File file = SPIFFS.open("/log.csv", FILE_APPEND);

    if (!file)
    {
        Serial.println("Cannot open CSV file");
        return;
    }

    file.print(millis() / 1000);
    file.print(",");

    file.print(bme.readTemperature(), 1);
    file.print(",");

    file.print(bme.readHumidity(), 1);
    file.print(",");

    file.println(bme.readPressure() / 100.0, 1);

    file.close();

    Serial.println("Data Logged");
}


void handleDownload()
{
    File file = SPIFFS.open("/log.csv", FILE_READ);

    if (!file)
    {
        server.send(404, "text/plain", "CSV file not found");
        return;
    }

    server.streamFile(file, "text/csv");
    file.close();
}


void setup()
{
    Serial.begin(115200);
    delay(1000);

    if (!SPIFFS.begin(true))
{
    Serial.println("SPIFFS Mount Failed");
    while (1);
}

Serial.println("SPIFFS Ready");


if (!SPIFFS.exists("/log.csv"))
{
    File file = SPIFFS.open("/log.csv", FILE_WRITE);

    if (file)
    {
        file.println("Time(s),Temperature(C),Humidity(%),Pressure(hPa)");
        file.close();

        Serial.println("CSV Header Created");
    }
}

    // WiFi
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to WiFi");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("==================================");
    Serial.println("WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("==================================");

server.begin();
Serial.println("Web Server Started");


    if (!bme.begin(0x76))
    {
        Serial.println("Could not find BME280 sensor!");
        while (1);
    }

    Serial.println("BME280 Initialized");

    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.on("/download", handleDownload);


}

void loop()
{
  server.handleClient();


  if (millis() - lastLog >= 10000)
{
    lastLog = millis();
    logData();
}

    Serial.print("Temperature: ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("Pressure: ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.println("------------------------");

    delay(100);
}