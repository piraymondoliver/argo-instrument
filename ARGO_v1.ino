#include <Stepper.h>

/////////////////////////////
// Define Global Constants //
/////////////////////////////

// Define Motor Steps Per Revolution
const int stepsperrev = 200;

// Define pins
const int Ain1 = 7;   // Green Wire   from Stepper Motor AIN1 to ItsyBitsy Pin 7
const int Ain2 = 9;   // Black Wire   from Stepper Motor AIN2 to ItsyBitsy Pin 9
const int Bin2 = 10;  // Blue wire    from Stepper Motor BIN2 to ItsyBitsy Pin 10
const int Bin1 = 11;  // Red wire     from Stepper Motor BIN1 to ItsyBitsy Pin 11

  
////////////////////////////////
// Instantiate Global Objects //
////////////////////////////////
Stepper stepperMotor = Stepper(stepsperrev, Ain1, Ain2, Bin2, Bin1); 


///////////
// Setup //
///////////
void setup()  // the setup function runs once when you press reset or power the board
{
    // Set Baud rate
    Serial.begin(9600);
  
    // initialize digital pin 13 as an output.
    pinMode(13, OUTPUT);

    // set speed of Stepper Motor (60 rpm)
    stepperMotor.setSpeed(120);
}


//////////
// Loop //
//////////   
void loop()   // the loop function runs over and over again forever   
{
    motorLEDTest();
}

////////////////////////////
// Test: Motor-Synced LED //
////////////////////////////
void motorLEDTest()
{
    digitalWrite(13, HIGH);   // turn the LED - pin 13 ON; physically mounted on ItsyBitsy Board (HIGH is the voltage level)
    stepperMotor.step(200);   // clockwise (CW) | 360* (1 Rev)
    Serial.println("Rotate Clockwise");
    digitalWrite(13, LOW);    // turn the LED - pin 13 ON; physically mounted on ItsyBitsy Board (by making the voltage LOW)
    delay(2000);              // wait for 2 seconds
  
    digitalWrite(13, HIGH);   // turn the LED - pin 13 ON
    stepperMotor.step(-200);  // counter-clockwise (CCW) | -360* (-1 Rev)
    Serial.println("Rotate Clockwise");
    digitalWrite(13, LOW);    // turn the LED - pin 13 OFF
    delay(2000);              // wait for 2 seconds
}
