#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// LCD
//#include <Wire.h>
//#include <LiquidCrystal_I2C.h>
//LiquidCrystal_I2C lcd(0x3f);
//0x3f

// WIFI credentials
const char* ssid = "secret";
const char* password = "secret";

ESP8266WebServer server(80);
const int led = 22;

////////////////////////////////
// Sensor Logic & Pins
////////////////////////////////
int power = true;

// Async
unsigned long previousMillis = 0;
const long interval = 500;
int errorState = LOW;

// Soil
const int hygrometer = A0;

// leds
volatile int q = 0; //initializing a integer for incrementing and decrementing duty ratio.
const int sa1 = 2; // treshold
const int sa2 = 13; // value

// touch sensor
int TouchSensor = 14;
int TouchLed = 0;
boolean currentState = LOW;
boolean lastState = LOW;
boolean LedState = LOW;

#include <Adafruit_Sensor.h>
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

  //lcd.begin(20, 4);
  //lcd.backlight();
  //delay(500);
  
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

  server.on("/plant", readings);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

}

void loop(void) {
  server.handleClient();

  readings();
  sync_leds();

  if (power == false) {
    digitalWrite(sa1, LOW);
    digitalWrite(sa2, LOW);
  }
  //async_leds();
  //stdby();
  delay(5000);
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

// Soil Variables
int value = 0;
int treshold = 20;

// memoization for faulty readings
int currentHumidity;
int currentTemperature;
int previousHumidity;
int previousTemperature;
int humid;
int temp;

// READINGS
int readings() {
  if (power == true) {

    // Hygrometer
    value = analogRead(hygrometer);
    value = constrain(value, 400, 1023);
    value = map(value, 400, 1023, 100, 0);

    // Humidity + Temperature
    float a = dht.readHumidity();
    // Read temperature as Celsius
    float b = dht.readTemperature() - 3;

    previousHumidity = currentHumidity;  // store what was read last time
    previousTemperature = currentTemperature;

    currentHumidity = a;  // get a new reading
    currentTemperature = b;

    // print outs
    if (!isnan(currentHumidity) || !isnan(currentTemperature)) {  // compare them
      humid = currentHumidity;
      temp = currentTemperature;
      serial_print();
      //lcd_out();
      wifi_out();
    } else {
      humid = previousHumidity;
      temp = previousTemperature;
      serial_print();
      //lcd_out();
      wifi_out();
    }
  }
}

// Serial OUT
int serial_print() {

  Serial.write(12); // clear terminal
  Serial.println("-----------------------");
  Serial.print(" Air Humidity: ");
  Serial.print(humid);
  Serial.println(" %\t");
  Serial.print(" Temperature: ");
  Serial.print(temp);
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
}

//WIFI OUT
int wifi_out() {
  String mesg = "";
  mesg += " Air Humidity: ";
  mesg += humid;
  mesg += " %\t";
  mesg += ";\n";
  mesg += " Temperature: ";
  mesg += temp;
  mesg += " *C ";
  mesg += ";\n";
  mesg += " Soil: ";
  mesg += value;
  mesg += "%";
  mesg += ";\n";

  server.send(200, "text/plain", mesg);
}

// synced leds
int sync_leds() {
  if (value < treshold) {
    analogWrite(sa2, LOW);
    analogWrite(sa1, LOW);
    delay(250);
    analogWrite(sa1, 55); // treshold led
    delay(250);
  } else {
    analogWrite(sa2, 55); // reading led
    analogWrite(sa1, LOW);
    //delay(5000);
  }
}

// LCD output
/*
  int lcd_out() {

  if (!isnan(humid)) {
    lcd.setCursor(0, 0);
    lcd.print("Humidity : ");
    lcd.print(humid);
    lcd.print(" %");
  }
  if (!isnan(temp)) {
    lcd.setCursor(0, 1);
    lcd.print("Temp     : ");
    lcd.print(temp);
    lcd.print(" C");
  }
  if (!isnan(value)) {
    lcd.setCursor(0, 2);
    lcd.print("Soil     : ");
    lcd.print(value);
    lcd.print("%");
    if (value  < treshold) {
      lcd.setCursor(0, 3);
      lcd.print("* Needs watering!! *");
    } else {
      lcd.setCursor(0, 3);
      lcd.print("                    ");
    }
  }
*/

// Led monitor async
int async_leds () {

  if (power == true) {
    unsigned long currentMillis = millis();

    if (value < treshold) {
      if (currentMillis - previousMillis >= interval) {
        digitalWrite(sa2, LOW);
        previousMillis = currentMillis;

        // Led Switch
        if (errorState == LOW) {
          errorState = HIGH;
        } else {
          errorState = LOW;
        }
        digitalWrite(sa1, errorState);
      }
    }  else {
      analogWrite(sa2, 5);
      digitalWrite(sa1, LOW);
    }
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
