#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// WIFI credentials
const char* ssid = "secret";
const char* password = "secret";

ESP8266WebServer server(80);

////////////////////////////////
// C O N F I G U R A T I O N  //
////////////////////////////////

int power = true;

// soil readings async
unsigned long previousMillis = 0;
const long interval = 15000;
const int inBetweenies = 100;   // delays between each read
const int numReadings = 15;     // Increase to smooth more, but will slow down readings

//    The interval between each read is 500ms, so 15 x 500 = 7500ms.
//    This needs to be lower than "interval" (15000ms + 1000ms), or
//    the readings will be unstable.

// * Serial print will notify if the limits are set wrong!

// dht readings async
unsigned long previousMillis2 = 0;
const long interval2 = 2000;

// soil
int treshold = 20; // 'dry'

const int hygrometer = A0; // pin data
const int hygroJuice = 15; // pin power

// leds
const int brightness = 55; // 0 = off, 255 = fully lit

const int sa1 = 2; // [treshold]
const int sa2 = 13; // [value]
const int sa3 = 0; // [standby]

// LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3f, 20, 4); // (memory address, columns, rows);

// capacative touch sensor
int TouchSensor = 14;
boolean currentState = LOW;
boolean lastState = LOW;
boolean LedState = LOW;

// Smoothing variables
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

// end config  //
///////////////////////////////////////////////////////////////////////////////

void handleRoot() {
  digitalWrite(sa3, 1);
  server.send(200, "text/plain", "/plant is where you find the data!");
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

  server.on("/plant", wifi_out);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  // smoothing init
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  // error handling for soil timings
  if ((numReadings * inBetweenies) + 1000 > interval) {
    Serial.println("ERROR: Your timing on 'interval' is ");
    Serial.println("too low for the amount of 'numReadings'");
    Serial.println("");
  }

  // inital read;
  // soil_readings();
  // serial_print();
}

void loop(void) {

  server.handleClient();

  unsigned long currentMillis = millis();
  unsigned long currentMillis2 = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    soil_readings();
    serial_print();
  }

  if (currentMillis2 - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis2;
    dht_readings();
  }

  // active modules
  sync_leds();
  lcd_out();
  wifi_out();
  stdby();

}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////
//final static int DELAY = 1000;
//int nextTimer, counter;

int value; // soil data

// memoization for faulty readings (dht11)
int currentHumidity;
int currentTemperature;
int previousHumidity;
int previousTemperature;
int humid;
int temp;

int reading_passes; // soil iteratiosn while  loop

// soil readings
int soil_readings() {

  // Hygrometer
  digitalWrite(hygroJuice, HIGH);
  delay(990);
  Serial.println("Amount of numreadings = ");
  Serial.println(numReadings);

  if (digitalRead(hygroJuice) == HIGH ) {

    reading_passes = 0;

    while (reading_passes <  numReadings ) {

      delay(inBetweenies);

      value = analogRead(hygrometer);
      value = constrain(value, 400, 1023);
      value = map(value, 1023, 400, 0, 100);

      smoothing(); // populate array

      reading_passes++;

      soil_phase_print();
      soil_error();
    }

    Serial.println("loop ended");
    Serial.print("Final total : ");
    Serial.println(total);
    readIndex = 0;
    delay(10);
    digitalWrite(hygroJuice, LOW);
  }

}

int dht_readings() {
  // Humidity + Temperature
  float a = dht.readHumidity();
  // Read temperature as Celsius
  float b = dht.readTemperature();

  previousHumidity = currentHumidity;
  previousTemperature = currentTemperature;

  if (!isnan(a) || !isnan(b)) {

    currentHumidity = a;
    currentTemperature = b;

    humid = currentHumidity;
    temp = currentTemperature;

  } else {

    humid = previousHumidity;
    temp = previousTemperature;

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

  if (power == true) {

    if (soil_avg < treshold) {
      analogWrite(sa2, 0);
      analogWrite(sa3, 0);

      analogWrite(sa1, brightness); // treshold led
      delay(100);
      analogWrite(sa1, 0);
      delay(100);
    } else {
      analogWrite(sa2, brightness);
      analogWrite(sa1, LOW);
    }
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
      lcd.backlight();

    } else {

      LedState = HIGH;
      power = false;

      // turn off leds and screen
      digitalWrite(sa1, LOW);
      digitalWrite(sa2, LOW);
      digitalWrite(sa3, LOW);
      lcd.noBacklight();

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

int soil_phase_print() {
  Serial.print("[ I T E R A T I O N : ");
  Serial.print(reading_passes );
  Serial.println(" ]");
  Serial.print("Current array index = ");
  Serial.println(readIndex);
  Serial.print("Current array value = ");
  Serial.println(readings[readIndex]);
  Serial.print("Current analog reading = ");
  Serial.print(value);
  Serial.println(" (direct)");
  Serial.print("Current total = ");
  Serial.println(total);
  Serial.println("");
};

// error handling for soil timings
int soil_error() {
  if ((numReadings * inBetweenies) + 1000 > interval) {
    Serial.println("");
    Serial.println("ERROR: Your timing on 'interval' is ");
    Serial.println("too low for the amount of 'numReadings'");
    Serial.println("");
  }
}
