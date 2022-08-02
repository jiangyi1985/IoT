// DS1302       Arduino     Nodemcu32s
// DAT/IO       4           25
// CLK/SCLK     5           26
// RST/CE       2           27
// VCC          3.3v/5v
// GND          GND

#include <ThreeWire.h>  
#include <RtcDS1302.h>//https://github.com/Makuna/Rtc

//ThreeWire myWire(4,5,2); // arduino - IO, SCLK, CE
ThreeWire myWire(25,26,27); // esp32 - IO, SCLK, CE

RtcDS1302<ThreeWire> Rtc(myWire);

void setup () 
{
    Serial.begin(115200);

    Serial.println("----------------------------------");
    Serial.println("code compile time: ");
    Serial.print(__DATE__);
    Serial.print(" ");
    Serial.println(__TIME__);

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();
    Serial.println();

    Rtc.Begin();

    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
        Serial.println("Rtc.IsDateTimeValid()==false. RTC lost confidence in the DateTime!");        
        Serial.println("Rtc.SetDateTime(compiled)...");
        Rtc.SetDateTime(compiled);
        Serial.println("Done.");
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("Rtc.GetIsWriteProtected()==true. RTC was write protected, enabling writing now...");
        Rtc.SetIsWriteProtected(false);
        Serial.println("Done.");
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("Rtc.GetIsRunning()==false. RTC was not actively running, starting now...");
        Rtc.SetIsRunning(true);
        Serial.println("Done.");
    }

    Serial.println("Checking time now...");
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();
    
    if (now < compiled) 
    {
        Serial.println("RTC < compile time!");
        Serial.println("Updating DateTime...");
        Rtc.SetDateTime(compiled);
        Serial.println("Done.");
    }
    else if (now > compiled) 
    {
        Serial.println("RTC > compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC == compile time! (not expected but all is fine)");
    }
    
    Serial.println();
}

void loop () 
{
    Serial.println("Reading...");
    
    Serial.print("Valid:");Serial.print(Rtc.IsDateTimeValid());
    Serial.print(" ReadOnly:");Serial.print(Rtc.GetIsWriteProtected());
    Serial.print(" Running:");Serial.println(Rtc.GetIsRunning());
    
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

    Serial.println();
    delay(3000);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u/%02u/%02u %02u:%02u:%02u"),
            dt.Year(),
            dt.Month(),
            dt.Day(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
    Serial.print(" ");
    Serial.print(dt);//seconds from year 2000
}
