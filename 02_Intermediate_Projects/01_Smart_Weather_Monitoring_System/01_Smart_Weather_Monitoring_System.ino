/****************************************************************************
 * File:        01_Smart_Weather_Monitoring_System.ino
 * Project:     Smart Weather Monitoring System
 * Author:      AYNOP Enterprises
 * Created:     31-12-2025
 * Version:     1.0
 * Target:      Arduino UNO R4 Minima 
 * IDE:         Arduino IDE 2.3.7 or later
 *
 * Description:
 *   Smart weather monitoring system using Arduino UNO R4 Minima.
 *   This project measures:
 *     - Temperature (°C)
 *     - Humidity (%)
 *     - Ambient Light Level (LDR)
 *
 * Revision History:
 *   v1.0 - Initial release
 *
 * License:
 *   This content (source code and documentation) is licensed under 
 *   Creative Commons BY-NC-SA 4.0. © 2025 AYNOP. You may use, modify, 
 *   and share for personal and educational purposes only. Commercial 
 *   use or redistribution without prior written permission is strictly 
 *   prohibited. Refer to the LICENSE file for complete details.
 *****************************************************************************/

#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <DHT.h>

/* -------------------- Pin Definitions -------------------- */
#define DHT_PIN     2
#define DHT_TYPE    DHT11
#define LDR_PIN     A0

/* -------------------- Objects -------------------- */
hd44780_I2Cexp lcd;
DHT dht(DHT_PIN, DHT_TYPE);

/* -------------------- Setup Function -------------------- */
void setup()
{
  lcd.begin(16, 2);
  lcd.backlight();

  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("AYNOP Weather");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

/* -------------------- Loop Function -------------------- */
void loop()
{
  float temperature = dht.readTemperature();   // keep decimal
  float humidityRaw = dht.readHumidity();      // %
  int   ldrRaw      = analogRead(LDR_PIN);     // 0–1023

  // Sensor error check
  if (isnan(temperature) || isnan(humidityRaw))
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error");
    lcd.setCursor(0, 1);
    lcd.print("Check Wiring");
    delay(2000);
    return;
  }

  // Humidity as integer (LCD friendly)
  int humidity = (int)humidityRaw;

  // Light percentage (reference logic)
  int lightPercent = map(ldrRaw, 0, 1023, 0, 100);
  lightPercent = constrain(lightPercent, 0, 100);

  // Line 1: Temperature (with decimal) & Humidity (%)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("%");

  // Line 2: Light percentage
  lcd.setCursor(0, 1);
  lcd.print("Light: ");
  lcd.print(lightPercent);
  lcd.print("%");

  delay(2000);
}
