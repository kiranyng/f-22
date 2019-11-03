// Include Libraries
#include "Arduino.h"
#include "Servo.h" 

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

int data[3];

// Pin Definitions
#define MOTOR_1_PIN_SIG 3 // D3
#define MOTOR_2_PIN_SIG 3
#define SERVO9G1_1_PIN_SIG 5 // D5
#define SERVO9G2_2_PIN_SIG 6 // D6

#define BATTERY_LEVEL_IN_PIN 17 // A0

// Global variables and defines
#define DATA_OUT_LEFT_TAIL 0
#define DATA_OUT_RIGHT_TAIL 1
#define DATA_OUT_THROTTLE 2

int planeBatteryLevel = 0;

// object initialization
Servo servo9g1_1;
Servo servo9g2_2;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() {
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial); // wait for serial port to connect. Needed for native USB

    radio.begin();
    radio.openWritingPipe(addresses[0]); // 00001
    radio.openReadingPipe(1, addresses[1]); // 00002
    radio.setPALevel(RF24_PA_MIN);

    data[DATA_OUT_LEFT_TAIL] = 0;
    data[DATA_OUT_RIGHT_TAIL] = 0;
    data[DATA_OUT_THROTTLE] = 0;

    servo9g1_1.attach(SERVO9G1_1_PIN_SIG);
    servo9g1_1.write(data[DATA_OUT_LEFT_TAIL]);
    //delay(100);
    //servo9g1_1.detach();

    servo9g2_2.attach(SERVO9G2_2_PIN_SIG);
    servo9g2_2.write(data[DATA_OUT_RIGHT_TAIL]);
    //delay(100);
    //servo9g2_2.detach();

    analogWrite(MOTOR_1_PIN_SIG, data[DATA_OUT_THROTTLE]);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() {
    delay(5);
    radio.startListening();
    if ( radio.available()) {
        while (radio.available()) {
            radio.read(&data, sizeof(data));

            Serial.println(" | input - T1, T2, Sp:" + String(data[DATA_OUT_LEFT_TAIL]) + ", "+ String(data[DATA_OUT_RIGHT_TAIL]) + ", " + String(data[DATA_OUT_THROTTLE]));
            
            servo9g1_1.write(data[DATA_OUT_LEFT_TAIL]);
            servo9g2_2.write(data[DATA_OUT_RIGHT_TAIL]);
            analogWrite( MOTOR_1_PIN_SIG, data[DATA_OUT_THROTTLE]);
        }

        delay(5);
        radio.stopListening();
        planeBatteryLevel = analogRead(BATTERY_LEVEL_IN_PIN);
        radio.write(&planeBatteryLevel, sizeof(planeBatteryLevel));
    }
}
