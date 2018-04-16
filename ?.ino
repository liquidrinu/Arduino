#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "secret";
const char* password = "secret";

ESP8266WebServer server(80);
const int led = 15;

////////////////////////////////
// Sensor Logic & Pins
////////////////////////////////
int power = true;

// Async
unsigned long previousMillis = 0;
const long interval = 500;
int errorState = LOW;

// leds
const int sa1 = 0; // treshold
const int sa2 = 2; // value

// touch sensor
int TouchSensor = 14;
int TouchLed = 13;
boolean currentState = LOW;
boolean lastState = LOW;
boolean LedState = LOW;

#include "DHT.h"
#define DHTPIN 12 // pinDATA
#define DHTTYPE DHT11 // sensor
DHT dht(DHTPIN, DHTTYPE);
////////////////////////////////

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  // sensor leds
  pinMode(sa1, OUTPUT);
  pinMode(sa2, OUTPUT);

  // touch led
  pinMode(TouchLed, OUTPUT);
  pinMode(TouchSensor, INPUT);

  // DHT11 sensor
  dht.begin();

  // webserver
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/sense", readings);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();

  stdby();
  readings();
  async_leds();
  //delay(100);
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

const int hygrometer = A0;
int value = 0;
int treshold = 20;

// READINGS
int readings() {
  if (power == true) {

    // Hygrometer
    value = analogRead(hygrometer);
    value = constrain(value, 400, 1023);
    value = map(value, 400, 1023, 100, 0);

    // Temperature + Humidity
    float x = dht.readHumidity();
    // Read temperature as Celsius
    float y = dht.readTemperature();

    // Serial Prints
    Serial.write(12); // clear terminal
    Serial.println("-----------------------");
    Serial.print(" Air Humidity: ");
    Serial.print(x);
    Serial.println(" %\t");
    Serial.print(" Temperature: ");
    Serial.print(y);
    Serial.println(" *C ");
    Serial.print(" Soil: ");
    Serial.print(value);
    Serial.print("%");

    if (value < treshold) {
      Serial.println(" ** Needs watering! **");
    } else {
      Serial.println("");
    }

    Serial.println("-----------------------");

    String mesg = "";
    mesg += " Air Humidity: ";
    mesg += x;
    mesg += " %\t";
    mesg += " Temperature: ";
    mesg += y;
    mesg += " *C ";
    mesg += "\n";
    mesg += " Soil: ";
    mesg += value;
    mesg += "%";
    mesg += "\n";

    server.send(200, "text/plain", mesg);
  }

  if (power == false) {
    digitalWrite(sa1, LOW);
    digitalWrite(sa2, LOW);
  }
}

// Standby
int stdby() {
  currentState = digitalRead(TouchSensor);
  if (currentState == HIGH && lastState == LOW) {
    delay(5);
    if (LedState == HIGH) {
      digitalWrite(TouchLed, LOW);
      LedState = LOW;
      power = true;
    } else {
      digitalWrite(TouchLed, HIGH);
      LedState = HIGH;
      power = false;
    }
  }
  lastState = currentState;
}

// Led monitpr async
int async_leds () {

  unsigned long currentMillis = millis();
  if (power == true) {
    if (value < treshold) {
      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // Led Switch
        if (errorState == LOW) {
          errorState = HIGH;
        } else {
          errorState = LOW;
        }
        digitalWrite(sa1, errorState);
      }
      digitalWrite(sa2, LOW);
    }  else {
      digitalWrite(sa2, HIGH);
      digitalWrite(sa1, LOW);
    }
  }
}
