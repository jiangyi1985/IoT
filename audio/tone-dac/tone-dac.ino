

const int dacPin = 25;  //Use GPIO 25 (DAC Channel 1)

// Calculate the half-period duration in microseconds: 1 second / (2 * frequency)
// For 255 Hz, this is approximately 1,960 microseconds per state
const int f1 = 255;  // The desired frequency in Hz
const unsigned long halfT1Us = 1000000 / (2 * f1);
const int f2 = 127;  // The desired frequency in Hz
const unsigned long halfT2Us = 1000000 / (2 * f2);

void setup() {

  Serial.begin(115200);
  // pinMode(PIN_TONE, OUTPUT);
  // No explicit setup needed for DAC pins, dacWrite handles initialization
}

void loop() {
  Serial.println("toning 1...");
  for (int i = 0; i < 255; i++) {
    // Generate a square wave with ~50% duty cycle
    dacWrite(dacPin, 255);            // High voltage (around 3.3V)
    delayMicroseconds(halfT1Us);  // Half period delay

    dacWrite(dacPin, 0);              // Low voltage (around 0V)
    delayMicroseconds(halfT1Us);  // Other half period delay
  }
  Serial.println("sleeping...");
  sleep(1);
  
  Serial.println("toning 2...");
  for (int i = 0; i < 255; i++) {
    // Generate a square wave with ~50% duty cycle
    dacWrite(dacPin, 255);            // High voltage (around 3.3V)
    delayMicroseconds(halfT2Us);  // Half period delay

    dacWrite(dacPin, 0);              // Low voltage (around 0V)
    delayMicroseconds(halfT2Us);  // Other half period delay
  }
  Serial.println("sleeping...");
  sleep(1);
}
