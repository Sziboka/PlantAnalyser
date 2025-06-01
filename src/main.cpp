#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <time.h>

#define IN_LED 2
#define SOIL 36
#define DHTPIN 23
#define DHTTYPE DHT22

#define THRESHOLD 1488

const char* ssid = "xxx";
const char* password = "xxx";
const char* serverUrl = "xxx"; 
//TODO ONE POST TO /SOIL WITH SOIL MOISTURE AND ONE TO /weather WITH HUMIDITY AND TEMPERATURE
//e.g 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  analogSetAttenuation(ADC_11db);


  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  pinMode(IN_LED, OUTPUT);
  digitalWrite(IN_LED, HIGH);
  delay(1000);
  digitalWrite(IN_LED, LOW);
  Serial.println("\nWiFi connected");

  // Configure NTP for Hungary (CET/CEST, Budapest)
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1); // Budapest uses CET/CEST
  tzset();
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync...");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println(" done!");

  // Set timezone for Hungary (CET/CEST, Budapest)
  setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
  tzset();
}

void loop() {


  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  int value = analogRead(SOIL); // read the analog value from sensor

  if (value > THRESHOLD)
    Serial.print("The soil is DRY (");
  else
    Serial.print("The soil is WET (");

  Serial.print(value);
  Serial.println(")");


  Serial.print("Moisture value: ");
  Serial.println(value);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    time_t now = time(nullptr);
    struct tm localTime;
    localtime_r(&now, &localTime);

    struct tm gmTime;
    gmtime_r(&now, &gmTime);

    // Calculate offset in seconds
    time_t localEpoch = mktime(&localTime);
    time_t gmEpoch = mktime(&gmTime);
    long offset = difftime(localEpoch, gmEpoch);

    int hours = offset / 3600;
    int mins = abs((offset % 3600) / 60);

    char dateTime[25];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%dT%H:%M:%S", &localTime);

    char isoTime[32];
    snprintf(isoTime, sizeof(isoTime), "%s%+03d:%02d", dateTime, hours, mins);

    String payload = "{\"humidity\":" + String(h) +
                    ",\"temp_c\":" + String(t) +
                    ",\"heat_index_c\":" + String(hic) +
                    ",\"moisture\":" + String(value) +
                    ",\"timestamp\":\"" + String(isoTime) + "\"}";
    Serial.println("Sending data to server: " + payload);

    int httpResponseCode = http.POST(payload);
    digitalWrite(IN_LED, HIGH);
    delay(2000);
    digitalWrite(IN_LED, LOW);
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
  delay(300000);
}