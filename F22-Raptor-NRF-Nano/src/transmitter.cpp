
// Include Libraries
#include "Arduino.h"

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

int data[3];

// Pin Definitions
#define PIN_LEFT_TAIL 14 // A0
#define PIN_RIGHT_TAIL 15 // A1
#define PIN_THROTTLE 17 // A3

#define DATA_OUT_LEFT_TAIL 0
#define DATA_OUT_RIGHT_TAIL 1
#define DATA_OUT_THROTTLE 2

// Global variables and defines

int planeBatteryLevel = 0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() {
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial); // wait for serial port to connect. Needed for native USB
    
    data[DATA_OUT_LEFT_TAIL] = 0;
    data[DATA_OUT_RIGHT_TAIL] = 0;
    data[DATA_OUT_THROTTLE] = 0;

    radio.openWritingPipe(addresses[1]); // 00002
    radio.openReadingPipe(1, addresses[0]); // 00001
    radio.setPALevel(RF24_PA_MIN);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() {
    delay(5);
    radio.stopListening();
    data[DATA_OUT_LEFT_TAIL] = map(analogRead(PIN_LEFT_TAIL), 0, 1023, 0, 180);
    data[DATA_OUT_RIGHT_TAIL] = map(analogRead(PIN_RIGHT_TAIL), 0, 1023, 0, 180);
    data[DATA_OUT_THROTTLE] = map(analogRead(PIN_THROTTLE), 0, 1023, 80, 255);

    Serial.println(String(analogRead(PIN_THROTTLE)) + " | input - T1, T2, Sp:" + String(data[DATA_OUT_LEFT_TAIL]) + ", "+ String(data[DATA_OUT_RIGHT_TAIL]) + ", " + String(data[DATA_OUT_THROTTLE]));
    
    radio.write(&data, sizeof(data));

    delay(5);
    radio.startListening();
    while (!radio.available());
    radio.read(&planeBatteryLevel, sizeof(planeBatteryLevel));
    if (planeBatteryLevel < 30) {
        Serial.write("LOW BATTERY WARNING!!");
    }
}
