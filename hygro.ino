//constants
const int hygrometer = A7;

int value = 0;
int treshold = 20;

// leds
const int sa1 = A0; // treshold
const int sa2 = A1; // value

#include "DHT.h"
#define DHTPIN 6 // pinDATA
#define DHTTYPE DHT11 // sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);
  dht.begin();

  pinMode(sa1, OUTPUT);
  pinMode(sa2, OUTPUT);
}

void loop() {

  // Temperature + Humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

  // Test
  if (isnan(h) || isnan(t) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Hygrometer
  value = analogRead(hygrometer);
  value = constrain(value, 400, 1023);
  value = map(value, 400, 1023, 100, 0);

  // Prints
  Serial.write(12); // clear terminal
  Serial.println("-----------------------");
  Serial.print(" Air Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print(" Temperature: ");
  Serial.print(t);
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

  // led monitor for soil
  if (value < treshold) {
    digitalWrite(sa2, LOW);
    for (int i = 0; i < 10; i++) {
      // amount of blinks
      for (int j = 0; j < 2; j++) {
        digitalWrite(sa1, HIGH);
        delay(100);
        digitalWrite(sa1, LOW);
        delay(100);
      }
      delay(600);
    }
  } else {
    digitalWrite(sa1, LOW);
    digitalWrite(sa2, HIGH);
    delay(10000);
  }

}
