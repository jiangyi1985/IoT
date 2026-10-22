#include <Wire.h>
#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for Serial Monitor to open (useful for ESP32)
  }

  Serial.println("AS7341 Standalone Flicker Detection Test");

  // Initialize AS7341 sensor on default I2C pins (GPIO 21 & 22 for NodeMCU-32S)
  if (!as7341.begin()) {
    Serial.println("Could not find an AS7341 sensor. Check wiring!");
    while (1) { 
      delay(10); 
    }
  }

  Serial.println("AS7341 Initialized Successfully.");
}

void loop() {
  // Read the ambient light flicker frequency
  uint16_t flickerHz = as7341.detectFlickerHz();

  // Print results to Serial Monitor
  Serial.print("Result: ");
  if (flickerHz == 0) {
    Serial.println("No flicker detected (Stable continuous light or darkness)");
  } else if (flickerHz == 1) {
    Serial.println("Unknown frequency / Unstable light cycle");
  } else {
    Serial.print(flickerHz);
    Serial.println(" Hz");
  }

  // Add a small pause between checks to avoid spamming the monitor
  delay(1000); 
}
