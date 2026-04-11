/****************************************************************************
 * File:        03_Flame_Alarm_System.ino
 * Project:     Flame Alarm System
 * Author:      AYNOP Enterprises
 * Created:     31-12-2025
 * Version:     1.0
 * Target:      Arduino UNO R4 Minima 
 * IDE:         Arduino IDE 2.3.7 or later
 *
 * Description:
 *   Flame alarm system using Arduino UNO R4 Minima.
 *   Once a flame is detected, the alarm is latched ON.
 *   The alarm can only be reset using a reset push button.
 *   Status is displayed on a 16x2 I2C LCD.
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
#define FLAME_PIN       8     // Flame sensor digital output
#define BUZZER_PIN      2     // Active buzzer
#define LED_PIN         13    // Onboard LED
#define RESET_BUTTON    6     // Reset push button

/* -------------------- LCD Object -------------------- */
hd44780_I2Cexp lcd;

/* -------------------- Alarm State -------------------- */
bool alarmLatched = false;

/* -------------------- Setup Function -------------------- */
void setup()
{
  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RESET_BUTTON, INPUT);  // Extrenal pull-up

  lcd.begin(16, 2);
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("AYNOP Flame");
  lcd.setCursor(0, 1);
  lcd.print("System Ready");
  delay(2000);
  lcd.clear();
}

/* -------------------- Loop Function -------------------- */
void loop()
{
  int flameState = digitalRead(FLAME_PIN);
  int resetState = digitalRead(RESET_BUTTON);

  // Flame detected (most modules output LOW)
  if (flameState == LOW)
  {
    alarmLatched = true;
  }

  // Reset button pressed
  if (resetState == LOW)
  {
    alarmLatched = false;
  }

  lcd.clear();

  if (alarmLatched)
  {
    lcd.setCursor(0, 0);
    lcd.print("FLAME DETECTED");
    lcd.setCursor(0, 1);
    lcd.print("RESET REQUIRED");

    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Status:");
    lcd.setCursor(0, 1);
    lcd.print("No Flame");

    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  delay(300);
}
