/****************************************************************************
 * File:        04_Joystick_Controlled_Robotic_Arm.ino
 * Project:     Joystick Controlled Robotic Arm
 * Author:      AYNOP Enterprises
 * Created:     04-03-2026
 * Version:     1.0
 * Target:      Arduino UNO R4 Minima 
 * IDE:         Arduino IDE 2.3.7 or later
 *
 * Description:
 *   Control a servo motor like a robotic arm using a joystick.
 *   Moving the joystick up/down smoothly changes the servo angle.
 *   When joystick is released, the arm holds its position.
 *****************************************************************************/

#include <Servo.h>

/* -------------------- Pin Definitions -------------------- */
#define JOYSTICK_Y_PIN   A0
#define SERVO_PIN        6

/* -------------------- Objects -------------------- */
Servo roboticArm;

/* -------------------- Variables -------------------- */
int currentAngle = 90;     // Start from center
int deadZoneMin  = 480;    // Joystick center stability
int deadZoneMax  = 540;

/* -------------------- Setup -------------------- */
void setup()
{
  roboticArm.attach(SERVO_PIN);
  roboticArm.write(currentAngle);
}

/* -------------------- Loop -------------------- */
void loop()
{
  int joystickValue = analogRead(JOYSTICK_Y_PIN);

  // Move arm UP
  if (joystickValue > deadZoneMax)
  {
    currentAngle++;
  }

  // Move arm DOWN
  else if (joystickValue < deadZoneMin)
  {
    currentAngle--;
  }

  // Limit angle between 0–180
  currentAngle = constrain(currentAngle, 0, 180);

  roboticArm.write(currentAngle);

  delay(15);   // smooth motion
}