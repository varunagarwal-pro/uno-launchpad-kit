/****************************************************************************
 * File:        05_RGB_Mood_Lamp.ino
 * Project:     RGB Mood Lamp (ON/OFF using Potentiometer)
 * Author:      AYNOP Enterprises
 * Created:     03-01-2026
 * Version:     1.0
 * Target:      Arduino UNO R4 Minima
 * IDE:         Arduino IDE 2.3.6 or later
 *
 * Description:
 *   Simple intermediate-level RGB Mood Lamp project.
 *   A potentiometer is used to switch RGB LED colors.
 *   Only one color is ON at a time using digital outputs.
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

// RGB LED pins
#define RED_PIN    11
#define GREEN_PIN  10
#define BLUE_PIN   9

// Potentiometer pin
#define POT_PIN   A0

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  int potValue = analogRead(POT_PIN);  // 0–1023

  // Turn all colors OFF first
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);

  // Select color based on potentiometer position
  if (potValue < 348) {
    digitalWrite(RED_PIN, HIGH);    // Red ON
  }
  else if (potValue > 813) {
    digitalWrite(GREEN_PIN, HIGH);  // Green ON
  }
  else {
    digitalWrite(BLUE_PIN, HIGH);   // Blue ON
  }

  delay(50);  // Stable switching
}
