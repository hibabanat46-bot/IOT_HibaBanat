#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WebServer.h>
#include <SPIFFS.h>

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
    icon = " ☀️";
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

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>THE WEATHER</title>

<style>

body{
    font-family:Arial;
    background:#f2f5f9;
    text-align:center;
    margin:0;
}

h1{
    background:#2196F3;
    color:white;
    padding:20px;
    margin:0;
}

.container{
    display:flex;
    justify-content:center;
    flex-wrap:wrap;
    margin-top:30px;
}

.card{
    width:220px;
    background:white;
    margin:15px;
    border-radius:15px;
    box-shadow:0 5px 15px rgba(0,0,0,0.2);
    padding:20px;
}

.value{
    font-size:35px;
    font-weight:bold;
    color:#2196F3;
}

.unit{
    font-size:20px;
}
    .weatherIcon{
    font-size:90px;
    margin-top:20px;
}
    .status{
    color:#f39c12;
    font-weight:bold;
    margin-bottom:20px;
}

.timestamp{
    margin-top:20px;
    font-size:18px;
    color:#555;
}
    .green{
    background:#c8f7c5;
}

.yellow{
    background:#fff3b0;
}

.red{
    background:#ffb3b3;
}

</style>

</head>
<script>

function updateData()
{
    fetch('/data')
    .then(response => response.json())
    .then(data => {

        document.getElementById("temp").innerHTML = data.temperature;

        document.getElementById("hum").innerHTML = data.humidity;

        document.getElementById("pres").innerHTML = data.pressure;

        // Temperature
let t = parseFloat(data.temperature);

document.getElementById("tempCard").className = "card";

if(t < 20)
    document.getElementById("tempCard").classList.add("green");
else if(t < 30)
    document.getElementById("tempCard").classList.add("yellow");
else
    document.getElementById("tempCard").classList.add("red");

// Humidity
let h = parseFloat(data.humidity);

document.getElementById("humCard").className = "card";

if(h >= 30 && h <= 60)
    document.getElementById("humCard").classList.add("green");
else if(h >= 20 && h <= 70)
    document.getElementById("humCard").classList.add("yellow");
else
    document.getElementById("humCard").classList.add("red");

// Pressure
let p = parseFloat(data.pressure);

document.getElementById("presCard").className = "card";

if(p >= 880&& p <= 920)
    document.getElementById("presCard").classList.add("green");
else if(p >= 860 && p <= 880)
    document.getElementById("presCard").classList.add("yellow");
else
    document.getElementById("presCard").classList.add("red");


        const now = new Date();
        document.getElementById("time").innerHTML =
        now.toLocaleTimeString();

    });
}

setInterval(updateData,10000);

</script>

<body>

<body>

<h1>🌍THE WEATHER</h1>

<div class="timestamp">
Last Update:
<span id="time">--:--:--</span>
</div>

)rawliteral";

html += "<div class='weatherIcon'>" + icon + "</div>";
html += "<h2 class='status'>" + status + "</h2>";

html += R"rawliteral(

<div class="container">

<div class="card" id="tempCard">
<h2>🌡 Temperature</h2>

<div class="value" id="temp">
)rawliteral";

  html += String(temperature,1);

  html += R"rawliteral(
</div>

<div class="unit">°C</div>

</div>

<div class="card" id="humCard">

<h2>💧 Humidity</h2>

<div class="value" id="hum">
)rawliteral";

  html += String(humidity,1);

  html += R"rawliteral(
</div>

<div class="unit">%</div>

</div>

<div class="card" id="presCard">

<h2>🌤 Pressure</h2>

<div class="value" id="pres">

)rawliteral";

  html += String(pressure,1);

  html += R"rawliteral(
</div>

<div class="unit">hPa</div>

</div>

</div>

<br><br>

<a href="/download">
<button style="
padding:15px 30px;
font-size:18px;
background:#2196F3;
color:white;
border:none;
border-radius:10px;
cursor:pointer;
">
Download CSV
</button>
</a>

</body>
</html>
)rawliteral";

  server.send(200,"text/html",html);
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

SPIFFS.remove("/log.csv");


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


Serial.println("Web Server Started");


    // BME280
    if (!bme.begin(0x76))
    {
        Serial.println("Could not find BME280 sensor!");
        while (1);
    }

    Serial.println("BME280 Initialized");

    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.on("/download", handleDownload);

server.begin();
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

    delay(2000);
}