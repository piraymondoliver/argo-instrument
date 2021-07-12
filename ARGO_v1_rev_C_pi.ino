#include <Wire.h>
#include <Stepper.h>
#include <Adafruit_AMG88xx.h>

/////////////////////////////////////////
// Define Global Constants & Variables //
/////////////////////////////////////////

    /////////////////                                           
    // Define Pins //
    /////////////////

    // Limit Switches
    #define limitSwitchOpen         A0;
    #define limitSwitchClosed       A1;
    #define opticalSwitch           A2;
    
    // Motor Stepper Inputs
    const int Ain1                  = 7;   // Green Wire   from Stepper Motor AIN1 to ItsyBitsy Pin 7
    const int Ain2                  = 9;   // Black Wire   from Stepper Motor AIN2 to ItsyBitsy Pin 9
    const int Bin2                  = 10;  // Blue wire    from Stepper Motor BIN2 to ItsyBitsy Pin 10
    const int Bin1                  = 11;  // Red wire     from Stepper Motor BIN1 to ItsyBitsy Pin 11

    // Limit Switches
    const int limitSwitchOpen       = 12;
    const int limitSwitchClosed     = 13;


    ////////////////////////////////////////
    // Define Motor Constants & Variables //
    ////////////////////////////////////////
            
        // Motor Constants
        const int steps_per_rev = 200;              // motor steps per revolution (from motor info)
        const float shutter_rotation_angle = 150;   // angle of required to open or close shutter
        const float deg_per_step = 1.8;             // degrees per step

        // Motor Variables    
        double shutter_rotation_steps = ( shutter_rotation_angle / deg_per_step );  // 200 steps  = 360* degrees
                                                                                    // 1 step     = 1.8* deg
                                                                                    // we need 150* of rotation
                                                                                    // =: x steps  = 150deg
                                                                                    // => x steps  = 150deg * (1 step / 1.8deg)
                                                                                    // => x steps  = (150/1.8) to fully open/close


    ////////////////////////////////////////////
    // Define IR Camera Constants & Variables //
    ////////////////////////////////////////////
    
        // Define Array of Pixels Variable to Hold an 8x8 Image
        float image[AMG88xx_PIXEL_ARRAY_SIZE];

        // Define IR Sensor Thermistor Temperature Variable
        float IR_sensor_temp;


    /////////////////////////////////////////////
    // Define Thermistor Constants & Variables //
    /////////////////////////////////////////////

        // Define Thermistor Temperature Variable to measure the motor temperature
        float motor_thermistor_temp;

  
    ////////////////////////////////
    // Instantiate Global Objects //
    ////////////////////////////////

        // Stepper Motor
        Stepper stepperMotor = Stepper(steps_per_rev, Ain1, Ain2, Bin2, Bin1);  // see above pins: Motor Stepper Inputs

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
    pinMode(13, OUTPUT);  // Multicolored_LED | On-board ItsyBitsy  | Pin 13
    

    // set speed of Stepper Motor (60 rpm)
    stepperMotor.setSpeed(120);
    return;
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
            case '3': motor_LED_Test();
                      break;
            case '4': IR_Camera_Test();
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
    stepperMotor.step(shutter_rotation_steps);   // Clockwise <CW> | +360/1.8[*] = +150[*] (+0.45 [Rev]) | Since our stepper motor turns 200 steps per revolution -
                                //                                                                       | divide 200 by the number of nths of a rev you wish to rotate:
                                //**********************************************************************************************************************************************//
                                //                                    |     // Positive Rotation Example | Clockwise (CW)                                                       //
                                //                                    |         e.g.:     +200 Steps     = +360* Degrees [*]    = +1 Revolution [Rev]  Clockwise <CW>           //
                                //                                    |                   +200/4 Steps   = +45*  Degrees        = +1 Rev               Clockwise <CW>           //
                                //                                    |                   therefore type "stepperMotor.step(200/4);" to rotate +45*    <CW>                     //
                                //**********************************************************************************************************************************************//
                                //                                    |     // Negative Rotation Example | Counter-Clockwise (CCW)                                              //
                                //                                    |         e.g.:   -200 Steps     = -360* Degrees [*]    = -1 Revolution [Rev]   Counter-Clockwise <CCW>   //
                                //                                    |                 -200/4 Steps   = -45*  Degrees        = -1 Rev                Counter-Clockwise <CCW>   //
                                //                                    |                 therefore type "stepperMotor.step(-200/4);" to rotate -45*    <CCW>                     //
                                //**********************************************************************************************************************************************//
    Serial.println("Shutter Opened! <Clockwise, CW>");
    return;
}


///////////////////////////////
// Subroutine: Close_Shutter //
///////////////////////////////
void close_Shutter()
{
    stepperMotor.step(-shutter_rotation_steps); // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
    Serial.println("Shutter Closed! <Counter-Clockwise, CCW>");
    return;
}


///////////////////////////////////
// Subroutine: take_IR_Image_8x8 //
///////////////////////////////////
void take_IR_Image_8x8()
{    
    //read all the pixels
    amg.readPixels(image);
    
    Serial.println("\t[");
    for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++)
    {
        Serial.print("\t");
        Serial.print(image[i-1]);
        Serial.print(", ");
        if( i%8 == 0 ) Serial.println();
    }
    Serial.println("\t]");
    //Serial.println();
    
    return;
}


////////////////////////////////////
// Subroutine: take_IR_Image_1x64 //
////////////////////////////////////
void take_IR_Image_1x64()
{    
    //read all the pixels
    amg.readPixels(image);
    
    Serial.print("[");
    for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++)
    {
        //Serial.print("");
        Serial.print(image[i-1]);
        Serial.print(", ");
        //if( i%8 == 0 ) Serial.println();
    }
    Serial.println("]");
    //Serial.println();
    
    return;
}


///////////////////////////////////////////
// Subroutine: IR Thermistor Temperature //
///////////////////////////////////////////
void IR_Thermistor_Temperature()
{
    // define local variables
    bool status;

    Serial.println(F("AMG88xx test"));

    // default settings
    status = amg.begin();
    if (!status) 
    {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }

    delay(100); // let sensor boot up

    // Read Thermistor Temperature from onboard AMG8833 Sensor
    IR_sensor_temp = amg.readThermistor();

    Serial.print("\tThermistor Temperature = ");
    Serial.print(IR_sensor_temp);
    Serial.println(" *C");      
    Serial.println();    
    //delay a second
    delay(1000);
    return;
}


////////////////////////////
// Test: Motor-Synced LED //
////////////////////////////
void motor_LED_Test()
{
    digitalWrite(13, HIGH);     // turn the LED - pin 13 ON; physically mounted on ItsyBitsy Board (HIGH is the voltage level)
    open_Shutter();                              
    digitalWrite(13, LOW);      // turn the LED - pin 13 ON; physically mounted on ItsyBitsy Board (by making the voltage LOW)
    delay(2000);                // wait for 2 seconds
    digitalWrite(13, HIGH);     // turn the LED - pin 13 ON
    close_Shutter();
    digitalWrite(13, LOW);      // turn the LED - pin 13 OFF
    delay(2000);                // wait for 2 seconds
    return;
}


//////////////////////////
// Test: IR Camera Test //
//////////////////////////
void IR_Camera_Test()
{
    Serial.println("-- Thermistor Test --");
    Serial.println();
    IR_Thermistor_Temperature();
    take_IR_Image_8x8();
    take_IR_Image_1x64();
    delay(1000);
    return;
}
