#include <IRremote.hpp>//https://github.com/Arduino-IRremote/Arduino-IRremote

int IR_RECEIVE_PIN=32;
void setup()
{
Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  if (IrReceiver.decode()) {
      IrReceiver.printIRResultShort(&Serial); // Print all fields
      IrReceiver.printIRResultRawFormatted(&Serial, true);  //Print the raw timing data received

      Serial.print("protocol:");
      Serial.print(IrReceiver.decodedIRData.protocol);
      Serial.print(" address:");
      Serial.print(IrReceiver.decodedIRData.address, HEX);
      Serial.print(" command:");
      Serial.print(IrReceiver.decodedIRData.command, HEX);
      Serial.print(" extra:");
      Serial.print(IrReceiver.decodedIRData.extra, HEX);
      Serial.print(" numberOfBits:");
      Serial.print(IrReceiver.decodedIRData.numberOfBits, HEX);
      Serial.print(" flags:");
      Serial.print(IrReceiver.decodedIRData.flags, HEX);
      Serial.print(" decodedRawData:");
      Serial.print(IrReceiver.decodedIRData.decodedRawData, HEX);
      Serial.println();
      Serial.print(" decodedRawDataArray:");
      Serial.print(IrReceiver.decodedIRData.decodedRawDataArray[0], HEX);
      Serial.print(" ");
      Serial.print(IrReceiver.decodedIRData.decodedRawDataArray[1], HEX);
      Serial.println();
      Serial.println();

      IrReceiver.resume(); // Enable receiving of the next value
  }
//  ...
}
