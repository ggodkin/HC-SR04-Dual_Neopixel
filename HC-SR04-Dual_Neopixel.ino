#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define NUM_LEDS 4  // Adjust the number of LEDs in your strip (4 to 8)
#define MAX_SENSOR_RANGE 400  // Maximum range of HC-SR04 sensor (in cm)

Adafruit_NeoPixel pixels(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

const int trigPin1 = 9;
const int echoPin1 = 8;
const int trigPin2 = 11;
const int echoPin2 = 10;

volatile long microsSinceTrig1 = 0, microsSinceTrig2 = 0;  // Use volatile for flags
volatile bool waitingForEcho1 = false, waitingForEcho2 = false;

int distance1 = 0, distance2 = 0, minDistance = 0;

void setup() {
  Serial.begin(9600);
  pixels.begin();  // Initialize NeoPixel library
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
}

void loop() {
  // Trigger distance measurement for sensor 1 if not already triggered
  if (!waitingForEcho1) {
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    microsSinceTrig1 = micros();
    waitingForEcho1 = true;
  }

  // Trigger distance measurement for sensor 2 if not already triggered
  if (!waitingForEcho2) {
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    microsSinceTrig2 = micros();
    waitingForEcho2 = true;
  }

  // Check if echo pulse received for sensor 1
  if (waitingForEcho1 && pulseIn(echoPin1, HIGH)) {
    distance1 = (micros() - microsSinceTrig1) * 0.034 / 2;
    waitingForEcho1 = false;
  }

  // Check if echo pulse received for sensor 2
  if (waitingForEcho2 && pulseIn(echoPin2, HIGH)) {
    distance2 = (micros() - microsSinceTrig2) * 0.034 / 2;
    waitingForEcho2 = false;
  }

  // Find the lowest distance (if both readings are available)
  if (distance1 > 0 && distance2 > 0) {
    minDistance = distance1 < distance2 ? distance1 : distance2;
  } else {
    // Use previously read distance if only one sensor has data
    minDistance = distance1 > 0 ? distance1 : distance2;
  }

  // Print distance for debugging (optional)
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.print(" cm, Distance2: ");
  Serial.print(distance2);
  Serial.print(" cm, Min Distance: ");
  Serial.println(minDistance);
  Serial.flush();

  // Set LED color based on distance range (assuming previous readings are available)
  // if (minDistance > 0) {  // Check if at least one sensor has a valid reading
    setColorByDistance(minDistance);
  // }

  // Show the LED strip
  pixels.show();

  delay(10);  // Short delay to avoid overwhelming loop
}

void setColorByDistance(int distance) {
  if (distance <= 30) {
    // Close (Red)
    pixels.fill(pixels.Color(255, 0, 0), 0);  //pixels.numPixels());
  } else if (distance <= 70) {
    // Mid-range (Orange)
    pixels.fill(pixels.Color(255, 165, 0), 0); //pixels.numPixels());
  } else {
    // Far (Green)
    pixels.fill(pixels.Color(0, 255, 0), 0); //pixels.numPixels());
  }
}
