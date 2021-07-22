
// ASCII Text Headers sourced from: "https://www.coolgenerator.com/ascii-text-generator" (Font: "Stick Letters")
#include <Arduino.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_AMG88xx.h>
#include <Adafruit_DotStar.h>

/*
#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define SYN 0x26
*/ 

///////////////////////////////////////////
//
//       __        __   __            
//      / _` |    /  \ |__)  /\  |    
//      \__> |___ \__/ |__) /~~\ |___ 
//

        /////////////////                                           
        // Define Pins //
        /////////////////

            // Optical Switch (Photoresistor) Input
            #define opticalSwitchPin            A0      // Brown Wire     from Open Switch Lead                   to ItsyBitsy Pin A0

            // Motor Thermistor Input
            #define motorThermistorPin          A1      // Green Wire     from Motor Thermistor Lead              to ItsyBitsy Pin A1

            // Limit Switch Inputs
            #define limitSwitchOpenPin          A2      // Orange Wire    from Open Position Limit Switch Lead    to ItsyBitsy Pin A2
            #define limitSwitchClosedPin        A3      // Purple Wire    from Closed Position Limit Switch Lead  to ItsyBitsy Pin A3

            // On-board DotStar LED
                // There is only one pixel on the ItsyBitsy M4 On-board DotStar LED 
                #define NUMPIXELS                   1 
                
                // Use these pin definitions for the ItsyBitsy M4 On-board DotStar LED
                #define CLOCKPIN                    6       // On-board                                               to ItsyBitsy Pin 6   
                #define DATAPIN                     8       // On-board                                               to ItsyBitsy Pin 8
                           
            // Motor Stepper Inputs
            const int Ain1Pin                   = 7;    // Green Wire     from Stepper Motor AIN1                 to ItsyBitsy Pin 7
            const int Ain2Pin                   = 9;    // Black Wire     from Stepper Motor AIN2                 to ItsyBitsy Pin 9
            const int Bin2Pin                   = 10;   // Blue wire      from Stepper Motor BIN2                 to ItsyBitsy Pin 10
            const int Bin1Pin                   = 11;   // Red wire       from Stepper Motor BIN1                 to ItsyBitsy Pin 11

            // On-board Alternate LED            
            const int ItsyBitsyLED              = 13;   // On-board                                               to ItsyBitsy Pin 13


        /////////////////                                           
        // Define Time //
        /////////////////

            // millisElapsed = milliseconds sincethe Arduino started
            unsigned long millisElapsed = 0;

        ////////////////////////////////////////
        // Define Motor Constants & Variables //
        ////////////////////////////////////////
                
            // Motor Constants
            const int   steps_per_rev           = 200;   // motor steps per revolution (from motor info)
            const int   shutter_rotation_angle  = 150;   // angle of required to open or close shutter
            const float deg_per_step            = 1.8;   // degrees per step

            // Motor Variables    
            double shutter_rotation_steps = 43;         // 200 steps  = 360* degrees
                                                        // 1 step     = 1.8* deg
                                                        // we need 150* of rotation
                                                        // =: x steps  = 150deg
                                                        // => x steps  = 150deg * (1 step / 1.8deg)
                                                        // => x steps  = (150/1.8) to fully open/close


        ////////////////////////////////
        // Define IR Camera Variables //
        ////////////////////////////////
        
            // Define Array of Pixels Variable to Hold an 8x8 Image
            float PROGMEM image[AMG88xx_PIXEL_ARRAY_SIZE] = { NULL };

            // Define IR Sensor Thermistor Temperature Variable
            float IR_sensor_temp = 0.0;


        /////////////////////////////////
        // Define Thermistor Variables //
        /////////////////////////////////

            // Define Thermistor Temperature Variable to measure the motor temperature
            float motor_thermistor_temp = 0.0;


        ///////////////////////////////////
        // Define Limit Switch Variables //
        ///////////////////////////////////

            // Define Limit Switch State Variables
            bool limitSwitchOpen    = false;
            bool limitSwitchClosed  = true;


        ///////////////////////////////////////////////////
        // Define Optical Switch Constants and Variables //
        ///////////////////////////////////////////////////

            // Define calibration threshold level - above this light level value the sensor will be allowed to be closed, below this light level value the sensor will be allowed to be opened
            int opticalCalibrationLevel = 950;

            // Define Optical Switch variable to measure the incoming light level (0-1023 values) 
            int opticalSwitchLightLevel = 0;

              
        ////////////////////////////////
        // Instantiate Global Objects //
        ////////////////////////////////

            // Stepper Motor
            AccelStepper stepperMotor = AccelStepper(AccelStepper::FULL4WIRE, Ain1Pin, Ain2Pin, Bin2Pin, Bin1Pin);  // see above pins: Motor Stepper Inputs

            // IR Camera
            Adafruit_AMG88xx amg;

            // On-board DotStar LED
            Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);
    


/////////////////////////////////////////////////////////////////////////////////
//
//            __  ___  __   __      ___            __  ___    __        __  
//      |\/| /  \  |  /  \ |__)    |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//      |  | \__/  |  \__/ |  \    |    \__/ | \| \__,  |  | \__/ | \| .__/ 
//

        //////////////////////////////
        // Subroutine: Open_Shutter //
        //////////////////////////////
        void open_Shutter()
        {   
            // Get Limit Switch States
            limitSwitchOpen = digitalRead(limitSwitchOpenPin);
            limitSwitchClosed = digitalRead(limitSwitchClosedPin);
            
            // Get the Light Level from the optical sensor
            opticalSwitchLightLevel = analogRead(opticalSwitchPin);
            
            // Wait 500 ms
            delay(500);
                        
            // Attempt to Open Shutter
            Serial.println(F("@@    * * * Attempting to Open the Shutter: * * *"));
            Serial.println(F("@@"));

            // If Shutter is Closed, then Open the Shutter
            if ( (limitSwitchClosed == true) && (limitSwitchOpen == false) && (opticalSwitchLightLevel < opticalCalibrationLevel) )
                {
                    // Turn the On-board Alternate LED on
                    digitalWrite(ItsyBitsyLED, HIGH);
                    
                    // Turn on the Rainbow function for the DotStar LED
                    strip.show();
                    strip.setBrightness(80);
                    rainbow(2);
        
                    // Wait 500 ms
                    delay(500);
          
                    stepperMotor.runToNewPosition(shutter_rotation_steps);   // Clockwise <CW> | +360/1.8[*] = +150[*] (+0.45 [Rev]) | Since our stepper motor turns 200 steps per revolution -
                                        //                                                                       | divide 200 by the number of nths of a rev you wish to rotate:
                                        //***********************************************************************************************************************************************//
                                        //                                    |     // Positive Rotation Example | Clockwise (CW)                                                        //
                                        //                                    |         e.g.:     +200 Steps     = +360* Degrees [*]    = +1 Revolution [Rev]  Clockwise <CW>            //
                                        //                                    |                   +200/4 Steps   = +45*  Degrees        = +1 Rev               Clockwise <CW>            //
                                        //                                    |                   therefore type "stepperMotor.step(200/4);" to rotate +45*    <CW>                      //
                                        //***********************************************************************************************************************************************//
                                        //                                    |     // Negative Rotation Example | Counter-Clockwise (CCW)                                               //
                                        //                                    |         e.g.:     -200 Steps     = -360* Degrees [*]    = -1 Revolution [Rev]   Counter-Clockwise <CCW>  //
                                        //                                    |                   -200/4 Steps   = -45*  Degrees        = -1 Rev                Counter-Clockwise <CCW>  //
                                        //                                    |                   therefore type "stepperMotor.step(-200/4);" to rotate -45*    <CCW>                    //
                                        //***********************************************************************************************************************************************//
                     Serial.println(F("@@    * * * SHUTTER OPENED! <Clockwise, CW> * * *"));
                     Serial.println(F("@@"));
                }
            else     // Else display an error message and do nothing    
            {
                Serial.println(F("@@    * * *      ERROR: Limit Switch Error!       * * *"));
                Serial.println(F("@@    * * *   Check if Shutter is already open    * * *"));
                Serial.println(F("@@"));
            }    
            
            // Disconnect power to the stepper motor
            stepperMotor.disableOutputs();

            // Wait 1 second
            delay(1000);
            
            return;
        }


        ///////////////////////////////
        // Subroutine: Close_Shutter //
        ///////////////////////////////
        void close_Shutter()
        {             
            // Get Limit Switch States
            limitSwitchOpen = digitalRead(limitSwitchOpenPin);
            limitSwitchClosed = digitalRead(limitSwitchClosedPin);
            
            // Get the Light Level from the optical sensor
            opticalSwitchLightLevel = analogRead(opticalSwitchPin);
               
            // Attempt to Close Shutter
            Serial.println(F("@@    * * * Attempting to Close the Shutter: * * *"));
            Serial.println(F("@@"));

            // Wait 500 ms
            delay(500);

            // If Shutter is Open, then Close the Shutter
            if ( (limitSwitchOpen == true) && (limitSwitchClosed == false) && (opticalSwitchLightLevel >= opticalCalibrationLevel) )
            {   
                // Turn the On-board Alternate LED off
                digitalWrite(ItsyBitsyLED, LOW);
                
                // Turn off the DotStar LED
                strip.setBrightness(0);
                strip.show();
                
                // Wait 500 ms
                delay(500); 
                
                stepperMotor.runToNewPosition(-shutter_rotation_steps);   // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
                Serial.println(F("@@    * * * SHUTTER CLOSED! <Counter-Clockwise, CCW> * * *"));
                Serial.println(F("@@"));
            }
            else     // Else display an error message and do nothing   
            {
                Serial.println(F("@@    * * *       ERROR: Limit Switch Error!        * * *"));
                Serial.println(F("@@    * * *   Check if Shutter is already closed    * * *"));
                Serial.println(F("@@"));
            }

            // Disconnect power to the stepper motor
            stepperMotor.disableOutputs();
            
            // Wait 1 second
            delay(1000);
            
            return;
        }


        ////////////////////////////////////////
        // Subroutine: EMERGENCY_Open_Shutter //
        ////////////////////////////////////////
        void EMERGENCY_Open_Shutter()
        {   
            // Turn the On-board Alternate LED on
            digitalWrite(ItsyBitsyLED, HIGH);
            
            // Turn on the Rainbow function for the DotStar LED
            strip.show();
            strip.setBrightness(80);
            rainbow(2);

            // Wait 500 ms
            delay(500);
          
            // Open the Shutter - ignoring the sensor readings
            stepperMotor.runToNewPosition(shutter_rotation_steps);   // Clockwise <CW> | +360/1.8[*] = +150[*] (+0.45 [Rev]) (see above commentary)
            Serial.println(F("@@    * * * EMERGENCY SHUTTER OPENED! <Clockwise, CW> * * *"));
            Serial.println(F("@@"));  

            // Disconnect power to the stepper motor
            stepperMotor.disableOutputs();
             
            // Wait 3 seconds
            delay(3000);
            
            return;
        }


        /////////////////////////////////////////
        // Subroutine: EMERGENCY_Close_Shutter //
        /////////////////////////////////////////
        void EMERGENCY_Close_Shutter()
        {   
            // Turn the On-board Alternate LED off
            digitalWrite(ItsyBitsyLED, LOW);

            // Turn off the DotStar LED
            strip.setBrightness(0);
            strip.show();
            
            // Wait 500 ms
            delay(500);
            
            // Open the Shutter - ignoring the sensor readings
            stepperMotor.runToNewPosition(-shutter_rotation_steps);   // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
            Serial.println(F("@@    * * * EMERGENCY SHUTTER CLOSED! <Counter-Clockwise, CCW> * * *"));
            Serial.println(F("@@"));

            // Disconnect power to the stepper motor
            stepperMotor.disableOutputs();
            
            // Wait 3 seconds
            delay(3000);
            
            return;
        }


        //////////////////////////////////////////////
        // Subroutine: Motor Thermistor Temperature //
        //////////////////////////////////////////////
        float Motor_Thermistor_Temperature()
        {
            // Declare and initialize local variables and constants
            int thermistor_Voltage = 0;
            float R0_therm = 100000.0;
            float T0_therm = 25.0;
            float B_val = 4000.0;
            float R_meas, Tk, Tc, Tf = 0.0;

            // Let sensor boot up (Wait 100 ms)
            delay(100); 
          
            thermistor_Voltage = analogRead(motorThermistorPin);
            R_meas = R0_therm * (1023.0 / (float)thermistor_Voltage - 1.0);
            
            Tc = (1.0 / ((1.0 / T0_therm) + (1.0/B_val) * log(R_meas / R0_therm)) );  // Celsius
            Tf = ((Tc * 9.0)/ 5.0 + 32.0);                                            // Fahrenheit
            Tk = (Tc + 273.15);                                                       // Kelvin

            // Print the Motor Thermistor Temperature
            Serial.print(F("@@    Motor Thermistor Temperature:        "));
            // Celsius
            Serial.print(Tc);
            Serial.print(F(" *C    "));
            // Fahrenheit 
            Serial.print(Tf);
            Serial.print(F(" *F    "));
            // Kelvin
            Serial.print(Tk);
            Serial.println(F(" K    "));
            Serial.println(F("@@")); 

            // Wait 900 ms
            delay(900);

            // Return the value of the temperature in degrees Celsius through the method name as a float
            return Tc;
        }



////////////////////////////////////////////////////////////////
//
//         __      ___            __  ___    __        __  
//      | |__)    |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//      | |  \    |    \__/ | \| \__,  |  | \__/ | \| .__/ 
//

        ///////////////////////////////////
        // Subroutine: take_IR_Image_8x8 //
        ///////////////////////////////////
        void take_IR_Image_8x8()
        {   
            // Declare and initialize local variables
            bool status = true;
            unsigned int hash_result = 0;

            // Default settings
            status = amg.begin();

            // Check if AMG8833 IR sensor is present
            if (!status) 
            {
                Serial.println(F("@@    Could not find a valid AMG88xx sensor, check wiring!"));
                Serial.println(F("@@"));
                while (1);
            }

            // Print a newline
            Serial.println(F("@@"));
            
            // Let sensor boot up (Wait 100 ms)
            delay(100);
             
            // Read all the pixels
            amg.readPixels(image);

            // Get the millis elapsed to use as a timestamp
            millisElapsed = millis();

            // Print the timestamp header
            Serial.print(F("$$ "));

            // Print the millis to use as a timestamp in Python
            Serial.println(millisElapsed);

            // Print the image header
            Serial.print(F("!! "));

            // Print the pixel array of 64 floats - comma-separated on eight lines (8x8 Matrix)           
            for(int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++)
            {
                Serial.print(image[i]);
                Serial.print(F(", "));
                if( i%8 == 0 )
                    Serial.print(F("!! ")); // Print a newline
            }            

            Serial.println(F("@@"));
            delay(100);             // Wait 100 ms

            //Serial.println(hash_result);
            hash_result = hash_IR_Image(image);
            delay(100);             // Wait 100 ms

            // Print the hash header
            Serial.print(F("## "));
            
            // Print the hash result
            Serial.println(hash_result);
            
            // Print a newline
            Serial.println(F("@@"));

            /*
            // Clean the image buffer out (may not be necessary - check before final release)
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
                image[i] = NULL ;
            */
            
            return;
        }


        ////////////////////////////////////
        // Subroutine: take_IR_Image_1x64 //
        ////////////////////////////////////
        void take_IR_Image_1x64()
        {   
            // Declare and initialize local variables
            bool status = true;
            unsigned int hash_result = 0;

            // Default settings
            status = amg.begin();

            // Check if AMG8833 IR sensor is present
            if (!status) 
            {
                Serial.println(F("@@    Could not find a valid AMG88xx sensor, check wiring!"));
                Serial.println(F("@@"));
                while (1);
            }
            
            // Print a newline
            Serial.println(F("@@"));
            
            // Let sensor boot up (Wait 100 ms)
            delay(100);
             
            // Read all the pixels
            amg.readPixels(image);
            
            // Get the millis elapsed to use as a timestamp
            millisElapsed = millis();

            // Print the timestamp header
            Serial.print(F("$$ "));

            // Print the millis to use as a timestamp in Python
            Serial.println(millisElapsed);

            // Print the image header
            Serial.print(F("!! "));
            
            // Print the pixel array of 64 floats - comma-separated on one line
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
            {
                Serial.print(image[i]);
                Serial.print(F(", "));
            }

            Serial.println(F("@@"));
            delay(100);             // Wait 100 ms

            //Serial.println(hash_result);
            hash_result = hash_IR_Image(image);
            delay(100);             // Wait 100 ms

            // Print the hash header
            Serial.print(F("## "));
            
            // Print the hash result
            Serial.println(hash_result);
            
            // Print a newline
            Serial.println(F("@@"));

            /*
            // Clean the image buffer out (may not be necessary - check before final release)
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
                image[i] = NULL ;
            */
            
            return;
        }


        ///////////////////////////////
        // Subroutine: Take_IR_Image //
        ///////////////////////////////
        void take_IR_Image()
        {
            IR_Thermistor_Temperature();
            //take_IR_Image_8x8();
            take_IR_Image_1x64();

            // Wait 1 second
            delay(1000);
            
            return;
        }


        ///////////////////////////////////////////
        // Subroutine: IR Thermistor Temperature //
        ///////////////////////////////////////////
        float IR_Thermistor_Temperature()
        {
            // Declare and initialize local variables
            bool status = true;
            float Tc, Tk, Tf = 0.0;

            // Default settings
            status = amg.begin();

            // Check if AMG8833 IR sensor is present
            if (!status) 
            {
                Serial.println(F("@@    Could not find a valid AMG88xx sensor, check wiring!"));
                // Print a newline
                Serial.println(F("@@"));
                while (1);
            }
            
            // Let sensor boot up (Wait 100 ms)
            delay(100);

            // Read Thermistor Temperature from onboard AMG8833 Sensor
            Tc = amg.readThermistor();    // Celsius              
            Tf = (Tc * 9.0)/ 5.0 + 32.0;  // Fahrenheit
            Tk = Tc + 273.15;             // Kelvin

            // Print the IR Sensor Thermistor Temperature
            Serial.print(F("@@    IR Sensor Thermistor Temperature:        "));
            // Celsius
            Serial.print(Tc);
            Serial.print(F(" *C    "));
            // Fahrenheit 
            Serial.print(Tf);
            Serial.print(F(" *F    "));
            // Kelvin
            Serial.print(Tk);
            Serial.println(F(" K    "));
            
            // Print a newline
            Serial.println(F("@@"));  

            // Wait 900 ms
            delay(900);

            // Return the value of the temperature in degrees Celsius through the method name as a float
            return Tc;
        }


        ///////////////////////////////
        // Subroutine: Hash IR Image //
        ///////////////////////////////
        unsigned int hash_IR_Image(float* image)
        {
            // Declare and initialize local variables and constants
            unsigned int     hash1             = 0;
            unsigned int     hash2             = 0;
            unsigned int     hash3             = 0;
            unsigned int     mersenne_prime    = 2147483647;   // A non-Mersenne Prime = (2^31)-1 which is modulo unsigned long range
            unsigned short   seed              = 48271;        // A good seed value from Spectral Test according to Knuth
            unsigned short   quotient          = 44488;        // (int)(mersenne_prime/seed)
            unsigned short   modulus           = 3399;         // mersenne_prime % 48271; it is important that modulus < quotient according to Knuth
            unsigned int     non_mers_prime    = 2147483399;
            unsigned short   seed2             = 40692;
            unsigned short   quot2             = 52774;
            unsigned short   modu2             = 3791;

            // Declare local pointers
            unsigned int*    temp_uint_ptr;
            float*           temp_float_ptr;
            
            // Make sure a valid string was passed in
            if (image == nullptr)
                return 0;
              
            else
            {
                // Cast key to unsigned int 
                // This ensures that elements of key will be treated as having integer values
                for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
                {
                    temp_float_ptr = &image[i];
                    //Serial.println(*temp_float_ptr);
                    temp_uint_ptr   = (unsigned int*)(temp_float_ptr);
                    //Serial.println(*temp_uint_ptr);
                    hash1 += (*temp_uint_ptr);
                    //Serial.println(hash1);
                }
                
                // Create two separate pre-hash values from our casted key            
                while (*temp_uint_ptr != NULL)
                {
                    hash1 = (hash1 * 2 + *temp_uint_ptr) % mersenne_prime;
                    hash2 = (hash2 * 3 + *temp_uint_ptr) % mersenne_prime;
                    *temp_uint_ptr++;
                }
            
                // From Knuth, Seminumerical Algorithms Vol. 2, 3rd ed., pp. 185 - 186
                hash1 = ((mersenne_prime * (hash1 % quotient)) - (modulus * (long)(hash1 / quotient)));
                if (hash1 < 0)
                    hash1 += mersenne_prime;
            
                hash2 = ((seed2 * (hash2 % quot2)) - (modu2 * (long)(hash2 / quot2)));
                if (hash2 < 0)
                    hash2 += non_mers_prime;
            
                hash3 = hash1 - hash2;
                if (hash3 <= 0)
                    hash3 += mersenne_prime;

                // Wait 500 ms
                delay(500);
                
                // Return a unique hash value through the method name as an unsigned int
                return hash3;
            } 
        }



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                     ___     __         ___  __           ___            __  ___    __        __  
//      |    |  |\/| |  |     /__` |  | |  |  /  ` |__|    |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//      |___ |  |  | |  |     .__/ |/\| |  |  \__, |  |    |    \__/ | \| \__,  |  | \__/ | \| .__/ 
//

        /////////////////////////////////////////
        // Subroutine: Limit Switch Open State //
        /////////////////////////////////////////
        void limit_Switch_Open_State()
        {
            // Wait 100 ms
            delay(100);
            
            // Get the HIGH or LOW voltage reading from the Limit Switch Open Pin
            limitSwitchOpen = digitalRead(limitSwitchOpenPin);
            limitSwitchClosed = !limitSwitchOpen;

            // Inform the user of the state of the Open Position Limit Switch
            if (limitSwitchOpen)
                Serial.println(F("@@    The Open Position Limit Switch is reading that the shutter is OPEN"));               
            else
                Serial.println(F("@@    The Open Position Limit Switch is reading that the shutter is CLOSED"));
            
            // Print a newline
            Serial.println(F("@@"));

            // Print the bool to the Serial Console
            Serial.print(F("@@    bool limitSwitchOpen = "));
            Serial.print(limitSwitchOpen);
            
            // Print a newline
            Serial.println(F(""));
            Serial.println(F("@@"));
            Serial.println(F("@@"));
            Serial.println(F("@@"));
           
            // Wait 900 ms
            delay(900);

            return;
        }
                

        ///////////////////////////////////////////
        // Subroutine: Limit Switch Closed State //
        ///////////////////////////////////////////
        void limit_Switch_Closed_State()
        {
            // Wait 100 ms
            delay(100);
            
            // Get the HIGH or LOW voltage reading from the Limit Switch Closed Pin
            limitSwitchClosed = digitalRead(limitSwitchClosedPin);
            limitSwitchOpen = !limitSwitchClosed;

            // Inform the user of the state of the Closed Position Limit Switch
            if (limitSwitchClosed)
                Serial.println(F("@@    The Closed Position Limit Switch is reading that the shutter is CLOSED"));               
            else
                Serial.println(F("@@    The Closed Position Limit Switch is reading that the shutter is OPEN"));

            // Print the bool to the Serial Console
            Serial.print(F("@@    bool limitSwitchClosed = "));
            Serial.print(limitSwitchClosed);
            
            // Print a newline
            Serial.println(F(""));
            Serial.println(F("@@"));
            Serial.println(F("@@"));
            Serial.println(F("@@"));

            // Wait 900 ms
            delay(900);
                
            return;
        }
        


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//       __   __  ___    __                __         ___  __           ___            __  ___    __        __  
//      /  \ |__)  |  | /  `  /\  |       /__` |  | |  |  /  ` |__|    |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//      \__/ |     |  | \__, /~~\ |___    .__/ |/\| |  |  \__, |  |    |    \__/ | \| \__,  |  | \__/ | \| .__/ 
//                                                                                                        

        //////////////////////////////////////
        // Subroutine: Optical Switch Level //
        //////////////////////////////////////
        int optical_Switch_Light_Level()
        {
            // Wait 100 ms
            delay(100);
            
            // Get the Light Level from the optical sensor
            opticalSwitchLightLevel = analogRead(opticalSwitchPin);

            // Print the Light Level (0-1023) to the Serial Console
            Serial.print(F("@@    int opticalSwitchLightLevel = "));
            Serial.print(opticalSwitchLightLevel);
            Serial.println(F(" in a range of [0, 1023]"));
            
            // Print a newline
            Serial.println(F("@@"));
            
            // Wait 900 ms
            delay(900);

            // return the value of the Light Level from the optical sensor through the method name as an int
            return opticalSwitchLightLevel;
        }



/////////////////////////////////////////////////////////////////////////
//            ___  __      ___            __  ___    __        __  
//      |    |__  |  \    |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//      |___ |___ |__/    |    \__/ | \| \__,  |  | \__/ | \| .__/ 
//

        /////////////////////////
        // Subroutine: Rainbow //
        /////////////////////////
        
        // Rainbow cycle along whole strip. Pass delay time (in ms) between frames. (From Example Library for AdaFruit DotStar: file "ItsyBitsyM4Onboard")
        void rainbow(int Wait) 
        {
            // Hue of first pixel runs 5 complete loops through the color wheel.
            // Color wheel has a range of 65536 but it's OK if we roll over, so
            // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
            // means we'll make 5*65536/256 = 1280 passes through this outer loop:
            for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) 
            {
                for(int i=0; i<strip.numPixels(); i++) 
                {   // For each pixel in strip...
                    // Offset pixel hue by an amount to make one full revolution of the
                    // color wheel (range of 65536) along the length of the strip
                    // (strip.numPixels() steps):
                    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
                    // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
                    // optionally add saturation and value (brightness) (each 0 to 255).
                    // Here we're using just the single-argument hue variant. The result
                    // is passed through strip.gamma32() to provide 'truer' colors
                    // before assigning to each pixel:
                    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
                }
                strip.show(); // Update strip with new contents
                delay(Wait);  // Pause for a moment
            }
        }



///////////////////////////////////////////////////////////////////////////
//
//      ___  ___  __  ___     ___            __  ___    __        __  
//       |  |__  /__`  |     |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//       |  |___ .__/  |     |    \__/ | \| \__,  |  | \__/ | \| .__/
//

        ////////////////////////////
        // Test: Motor-Synced LED //
        ////////////////////////////
        void motor_LED_Test()
        {
            open_Shutter();                              
            close_Shutter();
            return;
        }


        //////////////////////////
        // Test: IR Camera Test //
        //////////////////////////
        void IR_Camera_Test()
        {
            IR_Thermistor_Temperature();
            take_IR_Image_8x8();
            take_IR_Image_1x64();
            
            // Wait 1 second
            delay(1000);
            
            return;
        }



//////////////////////////////////////////////////////////////////////////
//
//                           ___            __  ___    __        __  
//      |\/|  /\  | |\ |    |__  |  | |\ | /  `  |  | /  \ |\ | /__` 
//      |  | /~~\ | | \|    |    \__/ | \| \__,  |  | \__/ | \| .__/
//

        ///////////
        // Setup //
        ///////////
        void setup()  // The setup function runs once when you press reset or power the board
        {
            // Set Baud rate
            Serial.begin(9600);

            // Wait 300 ms
            delay(300);
        
            // Initialize analog pins {A0, A1, A2, A3} as inputs.
            pinMode(opticalSwitchPin, INPUT);             // Optical Limit Switch         | Pin A0
            pinMode(motorThermistorPin, INPUT);           // Motor Thermistor             | Pin A1
            pinMode(limitSwitchOpenPin, INPUT_PULLUP);    // Open Position Limit Switch   | Pin A2
            pinMode(limitSwitchClosedPin, INPUT_PULLUP);  // Closed Position Limit Switch | Pin A3

            // Wait 300 ms
            delay(300);

            // Initialize On-board DotStar LED
            strip.begin(); // Initialize pins for output
            strip.setBrightness(80);
            strip.show();  // Turn all LEDs off ASAP

            // Wait 300 ms
            delay(300);
            
            // Initialize default values for maximum speed and maximum acceleration

            stepperMotor.setMaxSpeed(200); //SPEED = Steps / second
            stepperMotor.setAcceleration(100); //ACCELERATION = Steps /(second)^2
            stepperMotor.disableOutputs(); //disable outputs

            // Wait 300 ms
            delay(300);

            // Turn the On-board Alternate LED off
            digitalWrite(ItsyBitsyLED, LOW);
            
            // Wait 300 ms
            delay(300);

            // Greeting
            Serial.println(F("@@    +--------------------------------------------------+"));
            Serial.println(F("@@    |        Welcome to the ARGO IR Instrument!        |"));
            Serial.println(F("@@    +--------------------------------------------------+"));
            Serial.println(F("@@"));
            Serial.println(F("@@        * * * Press any key to load Main Menu: * * *"));
            Serial.println(F("@@"));
            Serial.println(F("@@    Default speed: 200 steps/s, default acceleration: 100 steps/s^2."));
            Serial.println(F("@@"));
            Serial.println(F("@@"));
            Serial.println(F("@@"));

            // Clears the input
            Serial.flush();
            
            // Wait 300 ms
            delay(300); 
            
            return;
        }


        ////////////////
        // Main: Loop //
        ////////////////
        void loop()   // The loop function runs over and over again forever   
        {
            // Define local variables:
            char menuChoice = NULL;

            Serial.flush();
            
            if (Serial.available() > 0)   // Constantly checks to see if anything has been sent over the USB Connection and if it needs to be processed
            {
                // Main Menu
                menuChoice = Serial.read(); // Reads a single letter

                // Present Menu Options
                Serial.println(F("@@    +-------------------+"));
                Serial.println(F("@@    |     Main Menu     |"));
                Serial.println(F("@@    +-------------------+"));
                Serial.println(F("@@    1.) Open Shutter"));
                Serial.println(F("@@    2.) Close Shutter"));
                Serial.println(F("@@    3.) Take IR Image"));
                Serial.println(F("@@    4.) Check Open Position Limit Switch Status"));
                Serial.println(F("@@    5.) Check Closed Position Limit Switch Status"));
                Serial.println(F("@@    6.) Check Optical Switch Light Level"));  
                Serial.println(F("@@    7.) Check IR Sensor Thermistor Temperature"));
                Serial.println(F("@@    8.) Check Motor Thermistor Temperature"));               
                Serial.println(F("@@    9.) IR Camera Test"));
                Serial.println(F("@@    0.) Motor-Synced LED Test"));
                Serial.println(F("@@"));
                Serial.println(F("@@    *******************************************"));
                Serial.println(F("@@    *                                         *"));
                Serial.println(F("@@    * * *       EMERGENCY  FUNCTIONS      * * *"));
                Serial.println(F("@@    *                                         *"));
                Serial.println(F("@@    *  Press '{' to force the Shutter Open    *"));
                Serial.println(F("@@    *  Press '}' to force the Shutter Closed  *"));
                Serial.println(F("@@    *******************************************"));
                Serial.println(F("@@"));
                Serial.println(F("@@"));                                      
                Serial.println(F("@@    * * * Press any key to Enter a Command or Refresh Main Menu * * *"));
                Serial.println(F("@@"));
                Serial.println(F("@@"));
                Serial.println(F("@@"));                          

                // If a valid Menu Choice was selected; display the selection back to the user
                if((menuChoice >= 48 && menuChoice <= 57) || menuChoice == 123 || menuChoice == 175) // if a valid menuChoice ASCII 0-9, '{', '}' or  was selected (see ASCII table for reference)
                {
                    Serial.print(F("@@    Menu Choice \""));
                    Serial.print(menuChoice);
                    Serial.println(F("\" was selected"));
                }
                else
                {
                    Serial.println(F("@@    * * * ERROR: Invalid Input! * * *"));
                    Serial.println(F("@@    * * *   Please Try Again    * * *"));
                    Serial.println(F("@@"));
                    Serial.print(F("@@    You typed:        \""));
                    if(menuChoice == 10)
                        Serial.print(F("ENTER/RETURN"));  // if ENTER was pressed, don't output the actual newline
                    else
                        Serial.print(menuChoice);   // else print the char
                    Serial.print(F("\"    DEC: "));
                    Serial.print(menuChoice, DEC);  // Print as an ASCII-encoded decimal
                    Serial.print(F("    HEX: "));
                    Serial.print(menuChoice, HEX);  // Print as an ASCII-encoded hexadecimal
                    Serial.print(F("    OCT: "));
                    Serial.print(menuChoice, OCT);  // Print as an ASCII-encoded octal
                    Serial.print(F("    BIN: "));
                    Serial.println(menuChoice, BIN);  // Print as an ASCII-encoded binary
                }

                Serial.println(F("@@"));
                Serial.println(F("@@"));
                Serial.println(F("@@"));
                
                switch(menuChoice)
                {
                    case '1': // Attempt to Open Shutter
                              open_Shutter();
                              break;
                    case '2': // Attempt to Close Shutter
                              close_Shutter();
                              break;
                    case '3': // Take an IR Image
                              take_IR_Image();
                              break;
                    case '4': // Check the Open Position Limit Switch State
                              limit_Switch_Open_State();
                              break;
                    case '5': // Check the Closed Position Limit Switch State
                              limit_Switch_Closed_State();
                              break;
                    case '6': // Check the Optical Switch Light Level
                              optical_Switch_Light_Level();
                              break;
                    case '7': // Check the IR Sensor Thermistor Temperature
                              IR_Thermistor_Temperature();
                              break;
                    case '8': // Check the Motor Thermistor Temperature
                              Motor_Thermistor_Temperature();
                              break;
                    case '9': // IR Camera Test Function
                              IR_Camera_Test();
                              break;          
                    case '0': // Motor Test Function 
                              motor_LED_Test();
                              break;
                    case '{': // EMERGENCY FUNCTION ONLY:
                              //    Force Open Shutter
                              EMERGENCY_Open_Shutter();
                              break;
                    case '}': // EMERGENCY FUNCTION ONLY:
                              //    Force Close Shutter
                              EMERGENCY_Close_Shutter();
                              break;                                 
                }

                // Wait 1 sec
                delay(1000); 

                Serial.println(F("@@    Returning to Main Menu"));
                Serial.println(F("@@    Please select one of the options listed:"));
                Serial.println(F("@@"));
                Serial.println(F("@@"));
                Serial.println(F("@@"));                
                                   
                Serial.flush(); // Clears the input
            }
        }
