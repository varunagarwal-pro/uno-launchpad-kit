/****************************************************************************
 * File:        02_Ultrasonic_Object_Detector.ino
 * Project:     Ultrasonic Object Detector
 * Author:      AYNOP Enterprises
 * Created:     31-12-2025
 * Version:     1.0
 * Target:      Arduino UNO R4 Minima 
 * IDE:         Arduino IDE 2.3.7 or later
 *
 * Description:
 *   Distance is measured using an HC-SR04 ultrasonic sensor and
 *   displayed on a 16x2 I2C LCD.
 *   To improve reliability, up to 3 readings are taken and the
 *   first valid distance value is used.
 *   An alert (LED + buzzer) is activated when an object comes
 *   within a defined distance.
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

/* -------------------- Pin Definitions -------------------- */
#define TRIG_PIN    12
#define ECHO_PIN    11
#define BUZZER_PIN  6
#define LED_PIN     13

/* -------------------- Alert Threshold -------------------- */
#define ALERT_DISTANCE_CM 20   // Alert when object is closer than this

/* -------------------- LCD Object -------------------- */
hd44780_I2Cexp lcd;

/* -------------------- Setup Function -------------------- */
void setup()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  lcd.begin(16, 2);
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("AYNOP Distance");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

/* -------------------- Distance Reading Function -------------------- */
float readDistance()
{
  // Trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration (timeout added)
  long duration = pulseIn(ECHO_PIN, HIGH, 70000);

  // Convert to distance in cm
  float distance = duration * 0.034 / 2;

  // Validate distance range
  if ((distance >= 1) && (distance <= 90))
  {
    return distance;
  }
  else
  {
    return -1;   // Invalid reading
  }
}

/* -------------------- Loop Function -------------------- */
void loop()
{
  float distance = -1;

  // Try up to 3 times to get a valid reading
  for (int i = 0; i < 3; i++)
  {
    distance = readDistance();
    if (distance != -1)
    {
      break;
    }
  }

  lcd.clear();

  if (distance != -1)
  {
    // Display distance
    lcd.setCursor(0, 0);
    lcd.print("Distance:");
    lcd.setCursor(0, 1);
    lcd.print(distance, 1);
    lcd.print(" cm");

    // Alert condition
    if (distance <= ALERT_DISTANCE_CM)
    {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
  else
  {
    // No valid object detected
    lcd.setCursor(0, 0);
    lcd.print("No Object");
    lcd.setCursor(0, 1);
    lcd.print("Detected");

    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(500);
}

