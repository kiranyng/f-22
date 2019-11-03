
// Include Libraries
#include "Arduino.h"
#include "Servo.h"

// Pin Definitions
#define MOTOR_1_PIN_SIG 3 // D3
#define MOTOR_2_PIN_SIG 3
#define SERVO9G1_1_PIN_SIG 5 // D5
#define SERVO9G2_2_PIN_SIG 6 // D6
#define IN_TAIL_1 14 // A0
#define IN_TAIL_2 15 // A1
#define IN_SPEED 17 // A3

// Global variables and defines
int tail1 = 0;
int tail2 = 0;
int speed = 0;

// object initialization
Servo servo9g1_1;
Servo servo9g2_2;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() {
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial); // wait for serial port to connect. Needed for native USB

    servo9g1_1.attach(SERVO9G1_1_PIN_SIG);
    servo9g1_1.write(tail1);
    //delay(100);
    //servo9g1_1.detach();

    servo9g2_2.attach(SERVO9G2_2_PIN_SIG);
    servo9g2_2.write(tail2);
    //delay(100);
    //servo9g2_2.detach();

    analogWrite(MOTOR_1_PIN_SIG, speed);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() {
    tail1 = map(analogRead(IN_TAIL_1), 0, 1023, 0, 180);
    tail2 = map(analogRead(IN_TAIL_2), 0, 1023, 0, 180);
    speed = map(analogRead(IN_SPEED), 0, 1023, 80, 255);

    Serial.println(String(analogRead(IN_SPEED)) + " | input - T1, T2, Sp:" + String(tail1) + ", "+ String(tail2) + ", " + String(speed));
    
    servo9g1_1.write(tail1);
    servo9g2_2.write(tail2);
    analogWrite( MOTOR_1_PIN_SIG, speed);
}
