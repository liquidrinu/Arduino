#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>

ESP8266WebServer server(80);
AutoConnect Portal(server);

////////////////////////////////
// C O N F I G U R A T I O N  //
////////////////////////////////

int power = true;

// soil readings async
unsigned long previousMillis_soil = 0;
const long interval_soil = 300000; // 5 minutes default

const int inBetweenies = 100;   // delays between each read
const int numReadings = 15;     // Increase to smooth more, but will slow down readings

//    The interval between each read is 100ms, so 15 x 100 = 1500ms.
//    This needs to be lower than "interval_soil" + 1000ms, or
//    the readings will be unstable. The added 1000ms is to give a bit
//    of leeway to the soilmeter's power input to suppres voltage spikes
//    on the first few reads

// * Serial print will notify if the limits are set wrong!

// dht readings async
unsigned long previousMillis_dht = 0;
const long interval_dht = 2000;

// soil
int treshold = 20; // 'dry'

const int hygrometer = A0; // pin data
const int hygroJuice = 15; // pin power

// leds
const int brightness = 55; // 0 = off, 255 = fully lit

const int sa1 = 2; // [treshold]
const int sa2 = 13; // [value]
const int sa3 = 0; // [standbyali]

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
  
  server.on("/", handleRoot);

  server.on("/plant", wifi_out);

  server.onNotFound(handleNotFound);

  Portal.begin();

  // smoothing init
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  // error handling for soil timings
  if ((numReadings * inBetweenies) + 1000 > interval_soil) {
    Serial.println("");
    Serial.println("ERROR: Your timing on 'interval' is ");
    Serial.print("too low for the amount of 'numReadings'");
    Serial.println("");
  }

  // TEST FUNCTION
  lcd.clear();
  test_init();
  dht_readings();
  soil_readings();
}

void loop(void) {

  Portal.handleClient();

  unsigned long currentMillis_soil = millis();
  unsigned long currentMillis_dht = millis();

  if (currentMillis_soil - previousMillis_soil >= interval_soil) {
    previousMillis_soil = currentMillis_soil;
    soil_readings();
    serial_print();
  }

  if (currentMillis_dht - previousMillis_dht >= interval_dht) {
    previousMillis_dht = currentMillis_dht;
    dht_readings();
  }

  // active modules
  sync_leds();
  lcd_out();
  wifi_out();
  stdby();

}

////////////////////////
// F U N C T I O N S  //
////////////////////////

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

  if (power == true) {
    analogWrite(sa3, brightness);
  }

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
    analogWrite(sa3, 0);
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
    readIndex = 0; // re-initialize array
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

// Initial test function
int ledPins[] = {sa1, sa2, sa3};
int currentLed;

int test_init() {
  for (int i = 0; i < 3; i++) {
    currentLed = ledPins[i];
    for (int j = 0; j < 10; j++) {
      analogWrite(currentLed, 0);
      delay(50);
      analogWrite(currentLed, 255);
      delay(50);
    }
  }
  digitalWrite(sa1, LOW);
  digitalWrite(sa2, LOW);
  digitalWrite(sa3, LOW);
}

// error handling for soil timings
int soil_error() {

  if ((numReadings * inBetweenies) + 1000 > interval_soil) {
    Serial.println("");
    Serial.print("ERROR: Your timing on 'interval' is too low to");
    Serial.println("do the amount of 'numReadings' you want");
    Serial.println("");
  }

}

// wifi reset
int wifiReset() {
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin();
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    WiFi.disconnect();
    while (WiFi.status() == WL_CONNECTED)
      delay(100);
  }
  Serial.println("WiFi disconnected.");
}
