//constants
const int hygrometer = A7;

// leds
const int sa1 = A0;
const int sa2 = A1;
const int sb1 = A2;
const int sb2 = A3;

int value;

#include "DHT.h"
#define DHTPIN 6     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(sa1, OUTPUT);
  pinMode(sa2, OUTPUT);
  pinMode(sb1, OUTPUT);
  pinMode(sb2, OUTPUT);
}

void loop() {

  //temperature
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // prints
  Serial.write(12);       // ESC command
  //Serial.write("[2j");  // clear terminal

  Serial.println("-----------------------");
  Serial.print(" Air Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print(" Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  // Hygrometer
  value = analogRead(hygrometer);
  value = constrain(value, 400, 1023); //Keep the ranges!
  value = map(value, 400, 1023, 100, 0);
  Serial.print(" Soil: ");
  Serial.print(value);
  Serial.println("%");
  Serial.println("-----------------------");

  // led monitor for soil
  if (value < 20) {
    digitalWrite(sa1, HIGH);
    digitalWrite(sa2, LOW);
  } else {
    digitalWrite(sa1, LOW);
    digitalWrite(sa2, HIGH);
  }
  delay(10000); //2 sec for humidity/temp
}

