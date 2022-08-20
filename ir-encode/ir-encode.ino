#include <IRremote.hpp>//https://github.com/Arduino-IRremote/Arduino-IRremote

int IR_SEND_PIN= 32;

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  IrSender.begin(IR_SEND_PIN); // Start with IR_SEND_PIN as send pin and if NO_LED_FEEDBACK_CODE is NOT defined, enable feedback LED at default feedback LED pin
}

/*
 * Set up the data to be sent.
 * For most protocols, the data is build up with a constant 8 (or 16 byte) address
 * and a variable 8 bit command.
 * There are exceptions like Sony and Denon, which have 5 bit address.
 */
uint16_t sAddress = 0xBF00;
uint8_t sCommand = 0x17;
uint8_t sRepeats = 0;
//vidda 85in DOWN: Protocol=NEC Address=0xBF00 Command=0x17 Raw-Data=0xE817BF00 32 bits LSB first
//vidda 85in UP:   Protocol=NEC Address=0xBF00 Command=0x16 Raw-Data=0xE916BF00 32 bits LSB first
// denon vol DOWN: Protocol=Kaseikyo_Denon Address=0x114 Command=0x17 Raw-Data=0x46171140 48 bits LSB first
// denon vol UP:   Protocol=Kaseikyo_Denon Address=0x14 Command=0x17 Raw-Data=0x56170140 48 bits LSB first


void loop() {
    //Print current send values
    Serial.println();
    Serial.print(F("Send now: address=0x"));
    Serial.print(sAddress, HEX);
    Serial.print(F(" command=0x"));
    Serial.print(sCommand, HEX);
    Serial.print(F(" repeats="));
    Serial.print(sRepeats);
    Serial.println();

    Serial.println(F("Send NEC with 16 bit address"));
    Serial.flush();

    IrSender.sendNEC(sAddress, sCommand, sRepeats);

//If you cannot avoid to send a raw value directly like e.g. 0xCB340102 you must use sendNECRaw()
//    Serial.println(F("Send NECRaw 0xE817BF00"));
//    IrSender.sendNECRaw(0xE817BF00, sRepeats);

     //Increment send values
     //Also increment address just for demonstration, which normally makes no sense
//    sAddress += 0x0101;
//    sCommand += 0x11;

    sRepeats++;
    // clip repeats at 4
    if (sRepeats > 4) {
        sRepeats = 0;
    }

    delay(1000);  // delay must be greater than 5 ms (RECORD_GAP_MICROS), otherwise the receiver sees it as one long signal
}
