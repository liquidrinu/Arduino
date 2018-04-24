#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// WIFI credentials
const char* ssid = "Woudvrucht";
const char* password = "Y0uSh@llN0tP@ss";

ESP8266WebServer server(80);

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 20, 4); // (memory address, columns, rows);

////////////////////////////////
// Sensor Logic & Pins
////////////////////////////////

int power = true;

// async readings
unsigned long previousMillis = 0;
const long interval = 1500;

// Soil
const int hygrometer = A0;
const int hygroJuice = 15;

// leds
const int brightness = 255; // 0 = off, 255 = fully lit
const int sa1 = 2; // [treshold]
const int sa2 = 13; // [value]
const int sa3 = 0; // [standby]

// capacative touch sensor
int TouchSensor = 14;

boolean currentState = LOW;
boolean lastState = LOW;
boolean LedState = LOW;

// Smoothing variables
const int numReadings = 10;     // amount of values used to average
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int soil_avg;                   // the smoothed output for all the functions

#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTPIN 12 // pinDATA
#define DHTTYPE DHT11 // sensor
DHT dht(DHTPIN, DHTTYPE);

////////////////////////////////

void handleRoot() {
  digitalWrite(sa3, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(sa3, 0);
}

void handleNotFound() {
  digitalWrite(sa3, 1);
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
  digitalWrite(sa3, 0);
}

void setup(void) {

  // lcd
  lcd.begin();
  lcd.backlight();

  // soil
  pinMode(hygroJuice, OUTPUT);
  digitalWrite(hygroJuice, LOW);

  // sensor leds
  pinMode(sa1, OUTPUT);
  pinMode(sa2, OUTPUT);
  pinMode(sa3, OUTPUT);

  // touch sensor (stdby)
  pinMode(TouchSensor, INPUT);

  // DHT11 sensor
  dht.begin();

  // webserver
  digitalWrite(sa3, 0);
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

  server.on("/plant", readings_total);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  // smoothing init
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop(void) {

  server.handleClient();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    readings_total();
  }

  stdby();

  if (power == false) {
    digitalWrite(sa1, LOW);
    digitalWrite(sa2, LOW);
    digitalWrite(sa3, LOW);
    lcd.noBacklight();
  } else {
    sync_leds();
    lcd.backlight();
  }

}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

// soil Variables
int value = 0;
int treshold = 20;

// memoization for faulty readings (dht11)
int currentHumidity;
int currentTemperature;
int previousHumidity;
int previousTemperature;
int humid;
int temp;

// READINGS
int readings_total() {

  // Hygrometer

  digitalWrite(hygroJuice, HIGH);
  delay(200);

  value = analogRead(hygrometer);
  value = constrain(value, 400, 1023);
  value = map(value, 1023, 400, 0, 100);

  delay(100);
  digitalWrite(hygroJuice, LOW);

  smoothing(); // soil value only

  // Humidity + Temperature
  float a = dht.readHumidity();
  // Read temperature as Celsius
  float b = dht.readTemperature();

  previousHumidity = currentHumidity;
  previousTemperature = currentTemperature;

  // print outs
  if (!isnan(a) || !isnan(b)) {

    currentHumidity = a;
    currentTemperature = b;

    humid = currentHumidity;
    temp = currentTemperature;

    serial_print();
    lcd_out();
    wifi_out();

  } else {

    humid = previousHumidity;
    temp = previousTemperature;

    serial_print();
    lcd_out();
    wifi_out();
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
  Serial.print(soil_avg);
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
  mesg += " Air Humidity : '";
  mesg += humid;
  mesg += "' %";
  mesg += ";\n";
  mesg += " Temperature' : '";
  mesg += temp;
  mesg += "' C";
  mesg += ";\n";
  mesg += " Soil         : '";
  mesg += soil_avg;
  mesg += "' %";
  mesg += ";\n";

  server.send(200, "text/plain", mesg);
}

// synced leds
int sync_leds() {

  if (soil_avg < treshold && power == true) {
    analogWrite(sa2, 0);
    analogWrite(sa1, brightness); // treshold led
    delay(100);
    analogWrite(sa1, 0);
    delay(100);
  } else {
    analogWrite(sa2, brightness); // reading led
    analogWrite(sa1, LOW);
  }

}

// LCD output
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
    lcd.print(soil_avg);
    lcd.print(" %");
    if (soil_avg  < treshold) {
      lcd.setCursor(0, 3);
      lcd.print("* Needs watering!! *");
    } else {
      lcd.setCursor(0, 3);
      lcd.print("                    ");
    }
  }
}

// Standby
int stdby() {

  currentState = digitalRead(TouchSensor);
  if (currentState == HIGH && lastState == LOW) {
    delay(5);
    if (LedState == HIGH) {
      LedState = LOW;
      power = true;
    } else {
      LedState = HIGH;
      power = false;
    }
  }
  lastState = currentState;
}

// Smoothing function (soil)
int smoothing() {

  total = total - readings[readIndex];   // subtract the last reading
  readings[readIndex] = value;  // read from the soil sensor
  total = total + readings[readIndex];  // add the reading to the total
  readIndex = readIndex + 1; // advance to the next position in the array

  if (readIndex >= numReadings) {
    readIndex = 0; // restart array
  }
  soil_avg = total / numReadings;
}
