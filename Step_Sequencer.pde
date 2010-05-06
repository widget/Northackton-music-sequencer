#include <Servo.h>

Servo myservo[4];  // create servo object to control a servo
byte beats[16];
byte beatNo = 0;
byte beatState = 0;

#define ledPin    13

// Declare the serial port variables
#define BUFSIZE   30
byte ReadState = 0;
byte buffer[BUFSIZE];
byte byteNo = 0;

unsigned long startMillis = 0;

/** Called once at the beginning of the sketch
 */
void setup()
{
    Serial.begin(19200);

    // Set up the LED as an indicator
    pinMode(ledPin, OUTPUT);

    myservo[0].attach(22);
    myservo[1].attach(23);
    myservo[2].attach(24);
    myservo[3].attach(25);

    startMillis = millis();
    memset(beats, 0, 16);
}

/** Called, unsurprisingly in a loop by the Arduino
 *  sketch.  Takes no arguments.
 */
void loop()
{
    unsigned long elapsedMillis = millis() - startMillis;

    // Check for any incomming commands
    ReadCommand();

    if(elapsedMillis > 100)
    {
        startMillis = millis();
        beatState ^= 0x01;

        if(beatState == 0)
        {
            OffBeat(beats[beatNo]);
            beatNo += 1;
            beatNo &= 0xF;
        }
        else
        {
            OnBeat(beats[beatNo]);
        }

        if(beatNo == 15)
            digitalWrite(ledPin, HIGH);
        else
            digitalWrite(ledPin, LOW);
    }
}

/// Twitch the servos if any should be on for this beat
void OnBeat(byte value)
{
    if((value & 0x01) == 0x01) myservo[0].write(80);
    if((value & 0x02) == 0x02) myservo[1].write(80);
    if((value & 0x04) == 0x04) myservo[2].write(80);
    if((value & 0x08) == 0x08) myservo[3].write(80);
}

/// Move back to defaults
void OffBeat(byte value)
{
    if((value & 0x01) == 0x01) myservo[0].write(100);
    if((value & 0x02) == 0x02) myservo[1].write(100);
    if((value & 0x04) == 0x04) myservo[2].write(100);
    if((value & 0x08) == 0x08) myservo[3].write(100);
}

void ReadCommand()
{
    if (Serial.available())
    {
        byte incoming = Serial.read();

        if(ReadState == 0)
        {
            if(incoming == '$')
            {
                byteNo = 0;
                ReadState = 1;
                //Serial.println("Read $");
            }
        }
        else if(ReadState == 1)
        {
            buffer[byteNo] = incoming;

            if((incoming == '\n') || (byteNo >= 16))
            {
                Serial.println("ok");

                for(int i = 0; i < 16; i += 1)
                {
                    beats[i] = HexToInt(buffer[i]);
                }

                //Serial.println("Zero Counters");
                ReadState = 0;
            }

            // If too many bytes arrive then reset
            byteNo += 1;

            if(byteNo > BUFSIZE)
            {
                byteNo = 0;
                ReadState = 0;
                Serial.println("Buf overflow");
            }
        }
    }
}

/// Quick ASCII hex reader
int HexToInt(byte ascii)
{
    if((ascii >= '0') && (ascii <= '9'))
        return ascii - 48;
    else if((ascii >= 'A') && (ascii <= 'F'))
        return ascii - 55;
    else
        return -1;
}
