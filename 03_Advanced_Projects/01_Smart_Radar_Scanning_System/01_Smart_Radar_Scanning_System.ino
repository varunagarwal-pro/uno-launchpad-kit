/****************************************************************************
 * File:        01_Smart_Radar_Scanning_System.ino
 * Project:     Smart Radar Scanning System
 * Author:      AYNOP Enterprises
 * Created:     22-09-2026
 * Version:     0.8.0
 * Target:      Arduino UNO R4 Minima
 * IDE:         Arduino IDE 2.3.7 or later
 *
 * Description:
 *   A radar scanning project using an SG90 Servo Motor and HC-SR04 
 *   Ultrasonic Sensor.
 *
 *   The servo continuously scans from 0° to 180° and back while the
 *   ultrasonic sensor checks for nearby objects.
 *
 *   System indication:
 *     - Green LED ON  : Area is clear.
 *     - Red LED ON    : Object detected.
 *     - Active Buzzer : Sounds when an object is detected.
 *     - LCD           : Displays the current system status.
 *
 *   A simple consecutive-reading filter helps reduce false detections
 *   caused by occasional unstable ultrasonic sensor readings.
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
 *
 ****************************************************************************/


/*==========================================================================*
 *                           LIBRARIES                                      *
 *==========================================================================*/

#include <Servo.h>
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>


/*==========================================================================*
 *                           OBJECTS                                        *
 *==========================================================================*/

// SG90 servo motor used for radar scanning.
Servo radarServo;

// 16x2 I2C LCD.
// The hd44780_I2Cexp class automatically detects the I2C backpack.
hd44780_I2Cexp lcd;


/*==========================================================================*
 *                         PIN DEFINITIONS                                  *
 *==========================================================================*/

// HC-SR04 Ultrasonic Sensor
const int ULTRASONIC_TRIGGER_PIN = 12;
const int ULTRASONIC_ECHO_PIN    = 11;

// SG90 Servo Motor
const int SERVO_PIN = 2;

// Status indicators
const int RED_LED_PIN   = 5;
const int GREEN_LED_PIN = 4;
const int BUZZER_PIN    = 8;


/*==========================================================================*
 *                       USER CONFIGURATION                                 *
 *==========================================================================*/

// Servo scanning range.
const int MIN_SCAN_ANGLE = 0;
const int MAX_SCAN_ANGLE = 180;

// Servo movement.
const int SERVO_STEP_DEGREES = 1;
const unsigned long SERVO_UPDATE_INTERVAL_MS = 15;

// Object detection range.
// An object at or below this distance is considered detected.
const int MAX_DETECTION_DISTANCE_CM = 20;

// Detection filter.
// The system must receive this many consecutive readings before
// changing the confirmed object status.
const int REQUIRED_DETECTION_READINGS = 3;
const int REQUIRED_CLEAR_READINGS     = 3;


/*==========================================================================*
 *                         GLOBAL VARIABLES                                 *
 *==========================================================================*/

// Current servo position.
int currentServoAngle = MIN_SCAN_ANGLE;

// Scanning direction:
//   1  = moving from 0° towards 180°
//  -1  = moving from 180° towards 0°
int servoScanDirection = 1;

// Counters used by the detection filter.
int detectionReadingCount = 0;
int clearReadingCount = 0;

// Confirmed object status after applying the filter.
bool objectDetected = false;

// Used to control servo movement without using delay().
unsigned long previousServoUpdateTime = 0;


/*==========================================================================*
 *                       FUNCTION PROTOTYPES                                *
 *==========================================================================*/

void updateServoPosition();
int measureObjectDistance();
void updateDetectionStatus(int measuredDistance);
void updateAlertIndicators();
void displayRadarStatus(int measuredDistance);


/*==========================================================================*
 *                              SETUP                                       *
 *==========================================================================*/

void setup()
{
    // Configure HC-SR04 pins.
    pinMode(ULTRASONIC_TRIGGER_PIN, OUTPUT);
    pinMode(ULTRASONIC_ECHO_PIN, INPUT);

    // Configure LED and buzzer pins.
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    // Keep the ultrasonic trigger LOW when the system starts.
    digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

    // Attach the servo and move it to the starting position.
    radarServo.attach(SERVO_PIN);
    radarServo.write(currentServoAngle);

    // Start the 16x2 I2C LCD.
    lcd.begin(16, 2);
    lcd.backlight();

    // Display a short startup message.
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Smart Radar");
    lcd.setCursor(1, 1);
    lcd.print("Initializing");

    delay(1500);

    lcd.clear();
}


/*==========================================================================*
 *                               LOOP                                       *
 *==========================================================================*/

void loop()
{
    // Step 1: Move the servo to the next scanning position.
    updateServoPosition();

    // Step 2: Measure the distance to an object.
    int measuredDistance = measureObjectDistance();

    // Step 3: Apply the detection filter.
    updateDetectionStatus(measuredDistance);

    // Step 4: Update the LED and buzzer.
    updateAlertIndicators();

    // Step 5: Update the LCD.
    displayRadarStatus(measuredDistance);
}


/*==========================================================================*
 *                     SERVO SCANNING                                      *
 *==========================================================================*/

/*
 * Moves the servo gradually between 0° and 180°.
 *
 * The servo direction is automatically reversed when either end
 * of the scanning range is reached.
 *
 * millis() is used instead of delay() so that the Arduino can
 * continue performing the other radar functions.
 */
void updateServoPosition()
{
    // Wait until the next servo update is due.
    if (millis() - previousServoUpdateTime < SERVO_UPDATE_INTERVAL_MS)
    {
        return;
    }

    previousServoUpdateTime = millis();

    // Move the servo to the current angle.
    radarServo.write(currentServoAngle);

    // Move to the next scanning angle.
    currentServoAngle += SERVO_STEP_DEGREES * servoScanDirection;

    // Reverse direction at the end of the scan.
    if (currentServoAngle >= MAX_SCAN_ANGLE)
    {
        currentServoAngle = MAX_SCAN_ANGLE;
        servoScanDirection = -1;
    }
    else if (currentServoAngle <= MIN_SCAN_ANGLE)
    {
        currentServoAngle = MIN_SCAN_ANGLE;
        servoScanDirection = 1;
    }
}


/*==========================================================================*
 *                    DISTANCE MEASUREMENT                                  *
 *==========================================================================*/

/*
 * Measures the distance using the HC-SR04 Ultrasonic Sensor.
 *
 * Returns:
 *   Distance in centimeters when a valid reading is received.
 *   -1 when no valid reading is available.
 */
int measureObjectDistance()
{
    // Send a short trigger pulse to the HC-SR04.
    digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(ULTRASONIC_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(ULTRASONIC_TRIGGER_PIN, LOW);

    // Measure the duration of the returned echo.
    // The timeout prevents the program from waiting indefinitely.
    unsigned long echoDuration = pulseIn(
        ULTRASONIC_ECHO_PIN,
        HIGH,
        30000
    );

    // No echo was received.
    if (echoDuration == 0)
    {
        return -1;
    }

    // Convert echo time into distance in centimeters.
    int distanceCM = echoDuration * 0.0343 / 2.0;

    // Reject readings outside the practical sensor range.
    if (distanceCM <= 0 || distanceCM > 400)
    {
        return -1;
    }

    return distanceCM;
}


/*==========================================================================*
 *                    OBJECT DETECTION FILTER                               *
 *==========================================================================*/

/*
 * Filters unstable ultrasonic readings.
 *
 * Object detection:
 *   3 consecutive object readings -> Object Detected
 *
 * Area clear:
 *   3 consecutive clear readings  -> Area Is Empty
 *
 * This prevents one incorrect ultrasonic reading from immediately
 * changing the LED, buzzer, and LCD status.
 */
void updateDetectionStatus(int measuredDistance)
{
    // Check whether the current reading is inside the detection range.
    bool currentReadingDetected =
        (measuredDistance > 0 &&
         measuredDistance <= MAX_DETECTION_DISTANCE_CM);

    if (currentReadingDetected)
    {
        // An object reading was received.
        detectionReadingCount++;

        // Reset the clear counter.
        clearReadingCount = 0;

        // Confirm object detection after the required readings.
        if (detectionReadingCount >= REQUIRED_DETECTION_READINGS)
        {
            objectDetected = true;

            // Keep the counter within its required limit.
            detectionReadingCount = REQUIRED_DETECTION_READINGS;
        }
    }
    else
    {
        // A clear reading was received.
        clearReadingCount++;

        // Reset the detection counter.
        detectionReadingCount = 0;

        // Confirm that the area is clear after the required readings.
        if (clearReadingCount >= REQUIRED_CLEAR_READINGS)
        {
            objectDetected = false;

            // Keep the counter within its required limit.
            clearReadingCount = REQUIRED_CLEAR_READINGS;
        }
    }
}


/*==========================================================================*
 *                    LED AND BUZZER CONTROL                                *
 *==========================================================================*/

/*
 * When the area is clear:
 *   Green LED = ON
 *   Red LED   = OFF
 *   Buzzer    = OFF
 *
 * When an object is detected:
 *   Green LED = OFF
 *   Red LED   = ON
 *   Buzzer    = ON
 */
void updateAlertIndicators()
{
    if (objectDetected)
    {
        // Object detected: activate the warning indicators.
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);
    }
    else
    {
        // No object detected: show the clear condition.
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(BUZZER_PIN, LOW);
    }
}


/*==========================================================================*
 *                         LCD DISPLAY                                      *
 *==========================================================================*/

/*
 * LCD when the area is clear:
 *
 *   Area Is Empty
 *   Angle : 90°
 *
 * LCD when an object is detected:
 *
 *   WARNING !!
 *   Dist  : 18 cm
 */
void displayRadarStatus(int measuredDistance)
{
    if (!objectDetected)
    {
        // First line: clear status.
        lcd.setCursor(0, 0);
        lcd.print("Area Is Empty   ");

        // Second line: current servo angle.
        lcd.setCursor(0, 1);
        lcd.print("Angle : ");

        // Add spaces so the angle remains aligned.
        if (currentServoAngle < 100)
        {
            lcd.print(" ");
        }

        if (currentServoAngle < 10)
        {
            lcd.print(" ");
        }

        lcd.print(currentServoAngle);
        lcd.print((char)223);
        lcd.print("    ");
    }
    else
    {
        // First line: warning status.
        lcd.setCursor(0, 0);
        lcd.print("OBJECT DETECTED!");

        // Second line: measured distance.
        lcd.setCursor(0, 1);
        lcd.print("Dist  : ");

        // Add spaces so the distance remains aligned.
        if (measuredDistance < 100)
        {
            lcd.print(" ");
        }

        if (measuredDistance < 10)
        {
            lcd.print(" ");
        }

        lcd.print(measuredDistance);
        lcd.print(" cm  ");
    }
}
