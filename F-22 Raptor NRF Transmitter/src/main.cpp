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

#define PIN_OUT_LOW_BATTERY 4 // D4
#define PIN_OUT_SIGNAL 5 // D4

#define DATA_OUT_LEFT_TAIL 0
#define DATA_OUT_RIGHT_TAIL 1
#define DATA_OUT_THROTTLE 2

#define BLINK_FAST 100
#define BLINK_MID 500
#define BLINK_SLO 1000

// Global variables and defines

int planeBatteryLevel = 0;
int blinkSpeed = 0;
int blinkTimer = 0;
boolean toggleOn = false;
boolean contacted = false;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup() {
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial); // wait for serial port to connect. Needed for native USB
    
    radio.begin();
    radio.openWritingPipe(addresses[1]); // 00002
    radio.openReadingPipe(1, addresses[0]); // 00001
    radio.setPALevel(RF24_PA_MIN);

    data[DATA_OUT_LEFT_TAIL] = 0;
    data[DATA_OUT_RIGHT_TAIL] = 0;
    data[DATA_OUT_THROTTLE] = 0;

    digitalWrite(PIN_OUT_LOW_BATTERY, LOW);
    digitalWrite(PIN_OUT_SIGNAL, LOW);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop() {
    digitalWrite(PIN_OUT_SIGNAL, LOW);
    blinkTimer++;

    delay(5);
    radio.stopListening();
    data[DATA_OUT_LEFT_TAIL] = map(analogRead(PIN_LEFT_TAIL), 0, 1023, 0, 180);
    data[DATA_OUT_RIGHT_TAIL] = map(analogRead(PIN_RIGHT_TAIL), 0, 1023, 180, 0);
    data[DATA_OUT_THROTTLE] = map(analogRead(PIN_THROTTLE), 0, 1023, 80, 255);

    Serial.println(String(analogRead(PIN_THROTTLE)) + " | input - T1, T2, Sp:" + String(data[DATA_OUT_LEFT_TAIL]) + ", "+ String(data[DATA_OUT_RIGHT_TAIL]) + ", " + String(data[DATA_OUT_THROTTLE]));
    
    radio.write(&data, sizeof(data));

    delay(5);
    radio.startListening();
    if (radio.available()) {
        digitalWrite(PIN_OUT_SIGNAL, HIGH);
        contacted = true;

        while (!radio.available());

        radio.read(&planeBatteryLevel, sizeof(planeBatteryLevel));
        Serial.write("Batter leve(0-1024):"+planeBatteryLevel);

        if (planeBatteryLevel < 720) { // ~3v | 650 -> 2.7v
            blinkSpeed = BLINK_SLO;
        }else if(planeBatteryLevel < 882){ // 3.7v
            blinkSpeed = BLINK_MID;
        }else if(planeBatteryLevel < 975){ // 4.25v
            blinkSpeed = BLINK_FAST;
        }
    }

    if(contacted && blinkTimer > blinkSpeed){
        blinkTimer = 0;
        toggleOn = (toggleOn == true)?false:false;
        Serial.write("BINK TOGGLE:"+toggleOn);
        
        if(toggleOn){
            digitalWrite(PIN_OUT_LOW_BATTERY, HIGH);
        }else{
            digitalWrite(PIN_OUT_LOW_BATTERY, LOW);
        }
    }else if(!contacted){
        digitalWrite(PIN_OUT_LOW_BATTERY, LOW);
        Serial.write("NOT CONTACTED!");
    }
}
