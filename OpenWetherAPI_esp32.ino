#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI myGLCD = TFT_eSPI();       // Invoke custom library

const char* ssid = "xxxxxx";
const char* password = "xxxxxx";
//Define API ke
String openWeatherMapApiKey = "191673ed63f3a3efxxxxxx";

String city = "xxxxx"; //specify your city
String countryCode = "xxxx"; //specify your country code

unsigned long last_time = 0;
unsigned long timer_delay = 10000;

String jsonBuffer;

void setup() {
  Serial.begin(115200);
  myGLCD.init();
  myGLCD.setRotation(1);
  myGLCD.fillScreen(TFT_BLACK);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TF_BL, HIGH);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WIFI...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("First set of readings will appear after 10 seconds");
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - last_time) > timer_delay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;

      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);

      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);// Reading in kelvin
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      
      myGLCD.fillScreen(TFT_BLACK);
      myGLCD.setTextColor(TFT_WHITE,TFT_BLACK);
      myGLCD.setTextDatum(TC_DATUM);
      
      myGLCD.drawString("T:",20,10,2);
      myGLCD.drawNumber(myObject["main"]["temp"],30,10,2);
      myGLCD.drawString("k",33,10,2);
      myGLCD.drawString("H:",20,20,2);                       //LCD Printing Humidity
      myGLCD.drawNumber(myObject["main"]["humidity"],30,20,2);
      myGLCD.drawString("%",33,20,2);
      myGLCD.drawString("P:"20,30,2);
      myGLCD.drawNumber(myObject["main"]["pressure"],30,30,2);
      myGLCD.drawString(" hPa",33,30,2);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    last_time = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
