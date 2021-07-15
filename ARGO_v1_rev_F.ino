// ASCII Text Headers sourced from: "https://www.coolgenerator.com/ascii-text-generator" (Font: "Stick Letters")
#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>
#include <Adafruit_AMG88xx.h>

//#define  AMG88xx_PIXEL_ARRAY_SIZE 64

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
            #define opticalSwitchPin            A0

            // Motor Thermistor Input
            #define motorThermistorPin          A1

            // Limit Switch Inputs
            #define limitSwitchOpenPin          A2
            #define limitSwitchClosedPin        A3
            
            // Motor Stepper Inputs
            const int Ain1Pin                   = 7;    // Green Wire   from Stepper Motor AIN1 to ItsyBitsy Pin 7
            const int Ain2Pin                   = 9;    // Black Wire   from Stepper Motor AIN2 to ItsyBitsy Pin 9
            const int Bin2Pin                   = 10;   // Blue wire    from Stepper Motor BIN2 to ItsyBitsy Pin 10
            const int Bin1Pin                   = 11;   // Red wire     from Stepper Motor BIN1 to ItsyBitsy Pin 11
    


        ////////////////////////////////////////
        // Define Motor Constants & Variables //
        ////////////////////////////////////////
                
            // Motor Constants
            const int   steps_per_rev           = 200;   // motor steps per revolution (from motor info)
            const int   shutter_rotation_angle  = 150;   // angle of required to open or close shutter
            const float deg_per_step            = 1.8;   // degrees per step

            // Motor Variables    
            float shutter_rotation_steps = (float)( shutter_rotation_angle / deg_per_step );  // 200 steps  = 360* degrees
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


        /////////////////////////////////////
        // Define Optical Switch Variables //
        /////////////////////////////////////

            // Define Optical Switch variable to measure the incoming light level (0-1023 values) 
            int opticalSwitchLightLevel = 0;

              
        ////////////////////////////////
        // Instantiate Global Objects //
        ////////////////////////////////

            // Stepper Motor
            Stepper stepperMotor = Stepper(steps_per_rev, Ain1Pin, Ain2Pin, Bin2Pin, Bin1Pin);  // see above pins: Motor Stepper Inputs

            // IR Camera
            Adafruit_AMG88xx amg;
    


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
            
            // Attempt to Open Shutter
            Serial.println(F("\t* * * Attempting to Open the Shutter: * * *\n"));
            
            // wait 1 second
            delay(1000);

            // If Shutter is Closed, then Open the Shutter
            if ((limitSwitchClosed == true) && (limitSwitchOpen == false) && (opticalSwitchLightLevel < 300))
                {
                    stepperMotor.step(shutter_rotation_steps);   // Clockwise <CW> | +360/1.8[*] = +150[*] (+0.45 [Rev]) | Since our stepper motor turns 200 steps per revolution -
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
                     Serial.println(F("\t* * * SHUTTER OPENED! <Clockwise, CW> * * *\n\n"));
                }
            else     // Else display an error message and do nothing    
            {
                Serial.println(F("\t* * *      ERROR: Limit Switch Error!       * * *"));
                Serial.println(F("\t* * *   Check if Shutter is already open    * * *\n\n"));
            }    
            
            // wait 2 seconds
            delay(2000);
            
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
            Serial.println(F("\t* * * Attempting to Close the Shutter: * * *\n"));

            // wait 1 second
            delay(1000);

            // If Shutter is Open, then Close the Shutter
            if ((limitSwitchOpen == true) && (limitSwitchClosed == false) && (opticalSwitchLightLevel > 301))
            {    
                stepperMotor.step(-shutter_rotation_steps);   // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
                Serial.println(F("\t* * * SHUTTER CLOSED! <Counter-Clockwise, CCW> * * *\n\n"));
            }
            else     // Else display an error message and do nothing   
            {
                Serial.println(F("\t* * *       ERROR: Limit Switch Error!        * * *"));
                Serial.println(F("\t* * *   Check if Shutter is already closed    * * *\n\n"));
            }
            
            // wait 2 seconds
            delay(2000);
            
            return;
        }


        ////////////////////////////////////////
        // Subroutine: EMERGENCY_Open_Shutter //
        ////////////////////////////////////////
        void EMERGENCY_Open_Shutter()
        {   
            // Open the Shutter - ignoring the sensor readings
            stepperMotor.step(shutter_rotation_steps);   // Clockwise <CW> | +360/1.8[*] = +150[*] (+0.45 [Rev]) (see above commentary)
            Serial.println(F("\t* * * EMERGENCY SHUTTER OPENED! <Clockwise, CW> * * *\n\n"));
             
            // wait 5 seconds
            delay(5000);
            
            return;
        }


        /////////////////////////////////////////
        // Subroutine: EMERGENCY_Close_Shutter //
        /////////////////////////////////////////
        void EMERGENCY_Close_Shutter()
        {   
            // Open the Shutter - ignoring the sensor readings
            stepperMotor.step(-shutter_rotation_steps);   // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
            Serial.println(F("\t* * * EMERGENCY SHUTTER CLOSED! <Counter-Clockwise, CCW> * * *\n\n"));
            
            // wait 5 seconds
            delay(5000);
            
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

            // let sensor boot up (wait 100 ms)
            delay(100); 
          
            thermistor_Voltage = analogRead(motorThermistorPin);
            R_meas = R0_therm * (1023.0 / (float)thermistor_Voltage - 1.0);
            
            Tc = (1.0 / ((1.0 / T0_therm) + (1.0/B_val) * log(R_meas / R0_therm)) );  // Celsius
            Tf = ((Tc * 9.0)/ 5.0 + 32.0);                                            // Fahrenheit
            Tk = (Tc + 273.15);                                                       // Kelvin

            // Print the Motor Thermistor Temperature
            Serial.print(F("\tMotor Thermistor Temperature:\t\t"));
            // Celsius
            Serial.print(Tc);
            Serial.print(F(" *C\t"));
            // Fahrenheit 
            Serial.print(Tf);
            Serial.print(F(" *F\t"));
            // Kelvin
            Serial.print(Tk);
            Serial.print(F(" K\t\n\n")); 

            // wait 3 seconds
            delay(3000);

            // return the value of the temperature in degrees Celsius through the method name as a float
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
            // declare and initialize local variables
            bool status = true;
            unsigned int hash_result = 0;

            // default settings
            status = amg.begin();

            // check if AMG8833 IR sensor is present
            if (!status) 
            {
                Serial.println(F("Could not find a valid AMG88xx sensor, check wiring!"));
                while (1);
            }
            
            // let sensor boot up (wait 100 ms)
            delay(100);
             
            //read all the pixels
            amg.readPixels(image);

            Serial.print(F("\n\t"));  // print a newline

            // Print the pixel array of 64 floats - comma-separated on eight lines (8x8 Matrix)           
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
            {
                Serial.print(image[i]);
                Serial.print(F(", "));
                if( i%8 == 0 )
                    Serial.print(F("\n")); // print a newline
            }            

            Serial.print(F("\n\t"));  // print a newline
            delay(100);             // wait 100 ms

            //Serial.println(hash_result);
            hash_result = hash_IR_Image(image);
            delay(100);             // wait 100 ms

            // Print the hash result
            Serial.println(hash_result);

            // clean the image buffer out
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
                image[i] = NULL ;
            
            return;
        }


        ////////////////////////////////////
        // Subroutine: take_IR_Image_1x64 //
        ////////////////////////////////////
        void take_IR_Image_1x64()
        {   
            // declare and initialize local variables
            bool status = true;
            unsigned int hash_result = 0;

            // default settings
            status = amg.begin();

            // check if AMG8833 IR sensor is present
            if (!status) 
            {
                Serial.println(F("\tCould not find a valid AMG88xx sensor, check wiring!"));
                while (1);
            }
            
            // let sensor boot up (wait 100 ms)
            delay(100);
             
            //read all the pixels
            amg.readPixels(image);
            
            Serial.print(F("\n\t"));  // print a newline
            
            // Print the pixel array of 64 floats - comma-separated on one line
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
            {
                Serial.print(image[i]);
                Serial.print(F(", "));
            }

            Serial.print(F("\n\t"));  // print a newline
            delay(100);             // wait 100 ms

            //Serial.println(hash_result);
            hash_result = hash_IR_Image(image);
            delay(100);             // wait 100 ms

            // Print the hash result
            Serial.println(hash_result);

            // clean the image buffer out
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
                image[i] = NULL ;
            
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

            // wait 3 seconds
            delay(3000);
            
            return;
        }


        ///////////////////////////////////////////
        // Subroutine: IR Thermistor Temperature //
        ///////////////////////////////////////////
        float IR_Thermistor_Temperature()
        {
            // declare and initialize local variables
            bool status = true;
            float Tc, Tk, Tf = 0.0;

            // default settings
            status = amg.begin();

            // check if AMG8833 IR sensor is present
            if (!status) 
            {
                Serial.println(F("Could not find a valid AMG88xx sensor, check wiring!"));
                while (1);
            }
            
            // let sensor boot up (wait 100 ms)
            delay(100);

            // Read Thermistor Temperature from onboard AMG8833 Sensor
            Tc = amg.readThermistor();    // Celsius              
            Tf = (Tc * 9.0)/ 5.0 + 32.0;  // Fahrenheit
            Tk = Tc + 273.15;             // Kelvin

            // Print the IR Sensor Thermistor Temperature
            Serial.print(F("\tIR Sensor Thermistor Temperature:\t\t"));
            // Celsius
            Serial.print(Tc);
            Serial.print(F(" *C\t"));
            // Fahrenheit 
            Serial.print(Tf);
            Serial.print(F(" *F\t"));
            // Kelvin
            Serial.print(Tk);
            Serial.print(F(" K\t\n\n"));  

            // wait 3 seconds
            delay(1000);

            // return the value of the temperature in degrees Celsius through the method name as a float
            return Tc;
        }


        ///////////////////////////////
        // Subroutine: Hash IR Image //
        ///////////////////////////////
        unsigned int hash_IR_Image(float* image)
        {
            // declare and initialize local variables and constants
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

            // declare local pointers
            unsigned int*    temp_uint_ptr;
            float*           temp_float_ptr;
            
            // Make sure a valid string was passed in
            if (image == nullptr)
                return 0;
              
            else
            {
                // cast key to unsigned int 
                // this ensures that elements of key will be treated as having integer values
                for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
                {
                    temp_float_ptr = &image[i];
                    //Serial.println(*temp_float_ptr);
                    temp_uint_ptr   = (unsigned int*)(temp_float_ptr);
                    //Serial.println(*temp_uint_ptr);
                    hash1 += (*temp_uint_ptr);
                    //Serial.println(hash1);
                }
                
                // create two separate pre-hash values from our casted key            
                while (*temp_uint_ptr != NULL)
                {
                    hash1 = (hash1 * 2 + *temp_uint_ptr) % mersenne_prime;
                    hash2 = (hash2 * 3 + *temp_uint_ptr) % mersenne_prime;
                    *temp_uint_ptr++;
                }
            
                // from Knuth, Seminumerical Algorithms Vol. 2, 3rd ed., pp. 185 - 186
                hash1 = ((mersenne_prime * (hash1 % quotient)) - (modulus * (long)(hash1 / quotient)));
                if (hash1 < 0)
                    hash1 += mersenne_prime;
            
                hash2 = ((seed2 * (hash2 % quot2)) - (modu2 * (long)(hash2 / quot2)));
                if (hash2 < 0)
                    hash2 += non_mers_prime;
            
                hash3 = hash1 - hash2;
                if (hash3 <= 0)
                    hash3 += mersenne_prime;

                // wait 500 ms
                delay(500);
                
                // return a unique hash value through the method name as an unsigned int
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
            // wait 100 ms
            delay(100);
            
            // Get the HIGH or LOW voltage reading from the Limit Switch Open Pin
            limitSwitchOpen = digitalRead(limitSwitchOpenPin);
            limitSwitchClosed = !limitSwitchOpen;

            // Inform the user of the state of the Open Position Limit Switch
            if (limitSwitchOpen)
                Serial.println(F("\tThe Open Position Limit Switch is reading that the shutter is OPEN"));               
            else
                Serial.println(F("\tThe Open Position Limit Switch is reading that the shutter is CLOSED"));

            // Print the bool to the Serial Console
            Serial.print(F("\tbool limitSwitchOpen = "));
            Serial.print(limitSwitchOpen);
            Serial.println(F("\n"));
           
            // wait 3 seconds
            delay(3000);

            return;
        }
                

        ///////////////////////////////////////////
        // Subroutine: Limit Switch Closed State //
        ///////////////////////////////////////////

        void limit_Switch_Closed_State()
        {
            // wait 100 ms
            delay(100);
            
            // Get the HIGH or LOW voltage reading from the Limit Switch Closed Pin
            limitSwitchClosed = digitalRead(limitSwitchClosedPin);
            limitSwitchOpen = !limitSwitchClosed;

            // Inform the user of the state of the Closed Position Limit Switch
            if (limitSwitchClosed)
                Serial.println(F("\tThe Closed Position Limit Switch is reading that the shutter is CLOSED"));               
            else
                Serial.println(F("\tThe Closed Position Limit Switch is reading that the shutter is OPEN"));

            // Print the bool to the Serial Console
            Serial.print(F("\tbool limitSwitchClosed = "));
            Serial.print(limitSwitchClosed);
            Serial.println(F("\n"));

            // wait 3 seconds
            delay(3000);
                
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
            // wait 100 ms
            delay(100);
            
            // Get the Light Level from the optical sensor
            opticalSwitchLightLevel = analogRead(opticalSwitchPin);

            // Print the Light Level (0-1023) to the Serial Console
            Serial.print(F("\tint opticalSwitchLightLevel = "));
            Serial.print(opticalSwitchLightLevel);
            Serial.println(F(" in a range of [0, 1023]\n"));
            
            // wait 3 seconds
            delay(3000);

            // return the value of the Light Level from the optical sensor through the method name as an int
            return opticalSwitchLightLevel;
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
            
            // wait 3 seconds
            delay(3000);
            
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
        void setup()  // the setup function runs once when you press reset or power the board
        {
            // Set Baud rate
            Serial.begin(9600);
        
            // initialize digital pins {0, 1, 2, 3, 13} as outputs.
            pinMode(opticalSwitchPin, INPUT);             // Optical Limit Switch         | Pin A0
            pinMode(motorThermistorPin, INPUT);           // Motor Thermistor             | Pin A1
            pinMode(limitSwitchOpenPin, INPUT_PULLUP);    // Open Position Limit Switch   | Pin A2
            pinMode(limitSwitchClosedPin, INPUT_PULLUP);  // Closed Position Limit Switch | Pin A3

            // set speed of Stepper Motor (60 rpm)
            stepperMotor.setSpeed(120);

            // wait 3 sec
            delay(3000);

            // Greeting
            Serial.println(F("+--------------------------------------------------+"));
            Serial.println(F("|        Welcome to the ARGO IR Instrument!        |"));
            Serial.println(F("+--------------------------------------------------+\n"));
            Serial.println(F("    * * * Press any key to load Main Menu: * * *\n"));
            return;
        }


        ////////////////
        // Main: Loop //
        ////////////////
        void loop()   // the loop function runs over and over again forever   
        {
            // Define local variables:
            char menuChoice = NULL;

            Serial.flush();
            
            while (Serial.available() > 0)   // Constantly checks to see if anything has been sent over the USB Connection and if it needs to be processed
            {
                // Present Menu Options
                Serial.println(F("\t+-------------------+"));
                Serial.println(F("\t|     Main Menu     |"));
                Serial.println(F("\t+-------------------+"));
                Serial.println(F("\t1.) Open Shutter"));
                Serial.println(F("\t2.) Close Shutter"));
                Serial.println(F("\t3.) Take IR Image"));
                Serial.println(F("\t4.) Check Open Position Limit Switch Status"));
                Serial.println(F("\t5.) Check Closed Position Limit Switch Status"));
                Serial.println(F("\t6.) Check Optical Switch Light Level"));  
                Serial.println(F("\t7.) Check IR Sensor Thermistor Temperature"));
                Serial.println(F("\t8.) Check Motor Thermistor Temperature"));               
                Serial.println(F("\t9.) IR Camera Test"));
                Serial.println(F("\t0.) Motor-Synced LED Test\n"));

                Serial.println(F("\t*******************************************"));
                Serial.println(F("\t* * *       EMERGENCY  FUNCTIONS      * * *"));
                Serial.println(F("\t*                                         *"));
                Serial.println(F("\t*  Press '{' to force the Shutter Open    *"));
                Serial.println(F("\t*  Press '}' to force the Shutter Closed  *"));
                Serial.println(F("\t*******************************************\n\n"));

                                                
                Serial.println(F("\t* * * Press any key to refresh Main Menu * * *\n\n"));
                

                // Main Menu
                    menuChoice = Serial.read(); // Reads a single letter

                    if((menuChoice >= 48 && menuChoice <= 57) || menuChoice == 123 || menuChoice == 175) // if a valid menuChoice ASCII 0-9, '{', '}' or  was selected (see ASCII table for reference)
                    {
                        Serial.print(F("\t-> Menu Choice \""));
                        Serial.print(menuChoice);
                        Serial.println(F("\" was selected\n"));
                    }
                    else
                    {
                        Serial.println(F("\t* * * ERROR: Invalid Input! * * *"));
                        Serial.println(F("\t* * *   Please Try Again    * * *\n\n"));
                    }
                    
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
    
                    Serial.println(F("\t-> Returning to Main Menu"));
                    Serial.println(F("\t-> Please select one of the options listed:\n\n\n"));
                                       
                    Serial.flush(); // Clears the input
                    
                    char menuChoice = NULL;
            }
        }
