#include <IRremote.hpp>//https://github.com/Arduino-IRremote/Arduino-IRremote

int IR_RECEIVE_PIN=32;
void setup()
{
Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial); // optional use new print version
//      ...
      IrReceiver.resume(); // Enable receiving of the next value
  }
//  ...
}
