#include <Wire.h>
#include <Stepper.h>
#include <Adafruit_AMG88xx.h>

/////////////////////////////
// Define Global Constants //
/////////////////////////////

// Define Motor Steps Per Revolution
const int stepsperrev = 200;

// Define pins
    // Indicator LED Lights
    const int Blue_LED    = 0;
    const int Green_LED   = 1;
    const int Yellow_LED  = 2;
    const int Red_LED     = 3;
    
    // Limit Switches
    const int limitSwitchOpen   = 4;
    const int limitSwitchClosed = 5;
    
    // Motor Stepper Inputs
    const int Ain1        = 7;   // Green Wire   from Stepper Motor AIN1 to ItsyBitsy Pin 7
    const int Ain2        = 9;   // Black Wire   from Stepper Motor AIN2 to ItsyBitsy Pin 9
    const int Bin2        = 10;  // Blue wire    from Stepper Motor BIN2 to ItsyBitsy Pin 10
    const int Bin1        = 11;  // Red wire     from Stepper Motor BIN1 to ItsyBitsy Pin 11

  
////////////////////////////////
// Instantiate Global Objects //
////////////////////////////////

    // Stepper Motor
    Stepper stepperMotor = Stepper(stepsperrev, Ain1, Ain2, Bin2, Bin1);  // see above pins: Motor Stepper Inputs

    // IR Camera
    Adafruit_AMG88xx amg;
 


///////////
// Setup //
///////////
void setup()  // the setup function runs once when you press reset or power the board
{
    // Set Baud rate
    Serial.begin(9600);
  
    // initialize digital pins {0, 1, 2, 3, 13} as outputs.
    pinMode(0,  OUTPUT);  // Blue_LED         | Breadboard Lane 40  | Pin A0
    pinMode(1,  OUTPUT);  // Green_LED        | Breadboard Lane 44  | Pin A1
    pinMode(2,  OUTPUT);  // Yellow_LED       | Breadboard Lane 48  | Pin A2
    pinMode(3,  OUTPUT);  // Red_LED          | Breadboard Lane 52  | Pin A3
    pinMode(13, OUTPUT);  // Multicolored_LED | On-board ItsyBitsy  | Pin 13

    // set speed of Stepper Motor (60 rpm)
    stepperMotor.setSpeed(120);
}


////////////////
// Main: Loop //
////////////////
void loop()   // the loop function runs over and over again forever   
{
    // Define local variables:
    char menuChoice;

    if (Serial.available())   // Constantly checks to see if anything has been sent over the USB Connection and if it needs to be processed
    {
        // Greeting
        Serial.println(F("Welcome to the ARGO IR Instrument!"));
    
        // Present Menu Options
        Serial.println(F("\t1.) Open Shutter"));
        Serial.println(F("\t2.) Close Shutter"));
        Serial.println(F("\t3.) Motor-Synced LED Test"));
        Serial.println(F("\t4.) IR Camera Test"));
        
        menuChoice = Serial.read(); // Reads a single letter
        switch(menuChoice)  // menu structure
        {
            case '1': open_Shutter();
                      break;
            case '2': close_Shutter();
                      break;
            case '3': motorLEDTest();
                      break;
            case '4': IRCameraTest();
                      Serial.print("Thermistor Temperature = ");
                      Serial.print(amg.readThermistor());
                      Serial.println(" *C");      
                      Serial.println();    
                      //delay a second
                      delay(1000);
                      break;
        }
        Serial.flush(); // Clears the input
    }
}


//////////////////////////////
// Subroutine: Open_Shutter //
//////////////////////////////
void open_Shutter()
{
     stepperMotor.step(200/4);  // Clockwise <CW> | +360/4* = +90* (+1/4 Rev)
}


///////////////////////////////
// Subroutine: Close_Shutter //
///////////////////////////////
void close_Shutter()
{
     stepperMotor.step(-200/4); // Counter-Clockwise <CCW> | -360/4* = -90* (-1/4 Rev)
}


////////////////////////////
// Test: Motor-Synced LED //
////////////////////////////
void motorLEDTest()
{
    digitalWrite(13, HIGH);     // turn the LED - pin 13 ON; physically mounted on ItsyBitsy Board (HIGH is the voltage level)
    stepperMotor.step(200/4);   // Clockwise <CW> | +360* (+1 Rev)    |     Since our stepper motor turns 200 steps per revolution - divide 200 by the number of nths of a 
                                //                                                                                            revolution you wish to rotate:
                                //**********************************************************************************************************************************************//
                                //                                    |     // Positive Rotation Example | Clockwise (CW)                                                       //
                                //                                    |         e.g.:     +200 Steps     = +360* Degrees [*]    = +1 Revolution [Rev]  Clockwise <CW>           //
                                //                                    |                   +200/4 Steps   = +45*  Degrees        = +1 Rev               Clockwise <CW>           //
                                //                                    |                   therefore type "stepperMotor.step(200/4);" to rotate +45*    <CW>                     //
                                //**********************************************************************************************************************************************//
                                //                                    |     // Negative Rotation Example | Counter-Clockwise (CCW)                                              //
                                //                                    |         e.g.:   -200 Steps     = -360* Degrees [*]    = -1 Revolution [Rev]   Counter-Clockwise <CCW>   //
                                //                                    |                 -200/4 Steps   = -45*  Degrees        = -1 Rev                Counter-Clockwise <CCW>   //
                                //                                    |                 therefore type "stepperMotor.step(-200/4);" to rotate -45*   <CCW>                      //
                                //**********************************************************************************************************************************************//
                              
    digitalWrite(13, LOW);      // turn the LED - pin 13 ON; physically mounted on ItsyBitsy Board (by making the voltage LOW)
    delay(2000);                // wait for 2 seconds
  
    digitalWrite(13, HIGH);     // turn the LED - pin 13 ON
    stepperMotor.step(-200/4);  // counter-Clockwise <CCW> | -360* (-1 Rev)
    Serial.println("Rotate Clockwise");
    digitalWrite(13, LOW);      // turn the LED - pin 13 OFF
    delay(2000);                // wait for 2 seconds
}

//
//
//
void IRCameraTest()
{
    // define local variables
    bool status;
    
    Serial.println(F("AMG88xx test"));

    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Thermistor Test --");
    Serial.println();

    delay(100); // let sensor boot up
}
