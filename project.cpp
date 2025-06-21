// Include necessary libraries
#include <DHT.h>           // For temperature and humidity sensor
#include <OneWire.h>       // For DS18B20 water temperature sensor
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h> // For LCD display (optional)

// Pin Definitions
#define DHTPIN 2         // DHT22 sensor data pin
#define DHTTYPE DHT22
#define WATER_TEMP_PIN 3 // DS18B20 data pin
#define PH_SENSOR_PIN A0
#define EC_SENSOR_PIN A1
#define WATER_LEVEL_PIN A2
#define PUMP_PIN 4
#define NUTRIENT_PUMP_PIN 5
#define LIGHT_PIN 6

// Sensor Objects
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(WATER_TEMP_PIN);
DallasTemperature waterTempSensor(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Optional

// Thresholds (adjust as needed)
float minWaterLevel = 300.0; // ADC value, calibrate as needed
float minTemp = 18.0;        // Celsius
float maxTemp = 24.0;        // Celsius
float minHumidity = 50.0;    // Percent
float maxHumidity = 70.0;    // Percent
float minEC = 1.2;           // mS/cm, for arugula
float maxEC = 2.0;
float minPH = 6.0;
float maxPH = 7.5;

void setup() {
  Serial.begin(9600);
  dht.begin();
  waterTempSensor.begin();
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(NUTRIENT_PUMP_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  // Optional: LCD initialization
  // lcd.init();
  // lcd.backlight();
}

void loop() {
  // Read sensors
  float airTemp = dht.readTemperature();
  float airHumidity = dht.readHumidity();
  waterTempSensor.requestTemperatures();
  float waterTemp = waterTempSensor.getTempCByIndex(0);

  int waterLevelRaw = analogRead(WATER_LEVEL_PIN);
  float waterLevel = (float)waterLevelRaw; // Calibrate as needed

  int phRaw = analogRead(PH_SENSOR_PIN);
  float phValue = map(phRaw, 0, 1023, 0, 14); // Simplified; calibrate with real probe

  int ecRaw = analogRead(EC_SENSOR_PIN);
  float ecValue = map(ecRaw, 0, 1023, 0, 5); // Simplified; calibrate with real probe

  // Control logic
  // Water level
  if (waterLevel < minWaterLevel) {
    digitalWrite(PUMP_PIN, HIGH); // Refill water
    Serial.println("Water low: Pump ON");
  } else {
    digitalWrite(PUMP_PIN, LOW);
  }

  // Nutrient dosing
  if (ecValue < minEC) {
    digitalWrite(NUTRIENT_PUMP_PIN, HIGH); // Dose nutrients
    Serial.println("EC low: Nutrient pump ON");
  } else {
    digitalWrite(NUTRIENT_PUMP_PIN, LOW);
  }

  // Lighting (e.g., 14 hours ON, 10 hours OFF)
  // For demonstration, always ON
  digitalWrite(LIGHT_PIN, HIGH);

  // Environmental alerts
  if (airTemp < minTemp || airTemp > maxTemp) {
    Serial.println("Warning: Air temperature out of range!");
  }
  if (airHumidity < minHumidity || airHumidity > maxHumidity) {
    Serial.println("Warning: Humidity out of range!");
  }
  if (waterTemp < minTemp || waterTemp > maxTemp) {
    Serial.println("Warning: Water temperature out of range!");
  }
  if (phValue < minPH || phValue > maxPH) {
    Serial.println("Warning: pH out of range!");
  }

  // Data logging
  Serial.print("Air Temp: "); Serial.print(airTemp); Serial.print(" C, ");
  Serial.print("Humidity: "); Serial.print(airHumidity); Serial.print(" %, ");
  Serial.print("Water Temp: "); Serial.print(waterTemp); Serial.print(" C, ");
  Serial.print("Water Level: "); Serial.print(waterLevel); Serial.print(", ");
  Serial.print("EC: "); Serial.print(ecValue); Serial.print(" mS/cm, ");
  Serial.print("pH: "); Serial.println(phValue);

  // Optional: Display on LCD
  // lcd.setCursor(0, 0);
  // lcd.print("T:"); lcd.print(airTemp); lcd.print("C H:"); lcd.print(airHumidity);
  // lcd.setCursor(0, 1);
  // lcd.print("pH:"); lcd.print(phValue); lcd.print(" EC:"); lcd.print(ecValue);

  delay(5000); // Adjust as needed
}
