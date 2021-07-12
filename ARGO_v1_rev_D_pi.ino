// ASCII Text Headers sourced from: "https: // www .coolgenerator.com/ascii-text-generator" (Font: "Stick Letters")
#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>
#include <Adafruit_AMG88xx.h>

#define  AMG88xx_PIXEL_ARRAY_SIZE 64

////////////////////////////////////////
//
//       __        __   __            
//      / _` |    /  \ |__)  /\  |    
//      \__> |___ \__/ |__) /~~\ |___ 
//

        /////////////////                                           
        // Define Pins //
        /////////////////

            // Limit Switches
            const int opticalSwitch         = A0;
            const int motorThermistor       = A1;
            
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
    



/////////////////////////////////////////////////////////////////////////////
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
            Serial.println(F("Shutter Opened! <Clockwise, CW>"));
            return;
        }


        ///////////////////////////////
        // Subroutine: Close_Shutter //
        ///////////////////////////////
        void close_Shutter()
        {
            stepperMotor.step(-shutter_rotation_steps); // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
            Serial.println(F("Shutter Closed! <Counter-Clockwise, CCW>"));
            return;
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
            unsigned int hash_result;
             
            //read all the pixels
            amg.readPixels(image);

            Serial.print(F("\n")); // print a newline

            // Print the pixel array of 64 floats - comma-separated on eight lines (8x8 Matrix)           
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
            {
                Serial.print(image[i]);
                Serial.print(F(", "));
                if( i%8 == 0 )
                    Serial.print(F("\n")); // print a newline
            }
            

            Serial.print(F("\n")); // print a newline
            delay(100);       // wait 100 ms

            //Serial.println(hash_result);
            hash_result = hash_IR_Image(image);
            delay(100);       // wait 100 ms

            // Print the hash result
            Serial.println(hash_result);
            
            return;
        }


        ////////////////////////////////////
        // Subroutine: take_IR_Image_1x64 //
        ////////////////////////////////////
        void take_IR_Image_1x64()
        {   
            unsigned int hash_result = 0;
             
            //read all the pixels
            amg.readPixels(image);
            
            Serial.print(F("\n")); // print a newline
            
            // Print the pixel array of 64 floats - comma-separated on one line
            for(int i = 0; i < AMG88xx_PIXEL_ARRAY_SIZE; i++)
            {
                Serial.print(image[i]);
                Serial.print(F(", "));
            }
            
            Serial.print(F("\n")); // print a newline
            delay(100);       // wait 100 ms

            //Serial.println(hash_result);
            hash_result = hash_IR_Image(image);
            delay(100);       // wait 100 ms

            // Print the hash result
            Serial.println(hash_result);
            
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
                Serial.println(F("Could not find a valid AMG88xx sensor, check wiring!"));
                while (1);
            }

            delay(100); // let sensor boot up

            // Read Thermistor Temperature from onboard AMG8833 Sensor
            IR_sensor_temp = amg.readThermistor();

            Serial.print(F("\tThermistor Temperature = "));
            Serial.print(IR_sensor_temp);
            Serial.println(F(" *C"));      
            Serial.print(F("\n"));
            //delay a second
            delay(1000);
            return;
        }


        ///////////////////////////////
        // Subroutine: Hash IR Image //
        ///////////////////////////////
        unsigned int hash_IR_Image(float* image)
        {
            unsigned int     hash1             = 0;
            unsigned int     hash2             = 0;
            unsigned int     hash3             = 0;
            unsigned int     mersenne_prime    = 2147483647;   // A non-Mersenne Prime = (2^31)-1 which is modulo unsigned long range
            unsigned short    seed              = 48271;        // A good seed value from Spectral Test according to Knuth
            unsigned short    quotient          = 44488;        // (int)(mersenne_prime/seed)
            unsigned short    modulus           = 3399;         // mersenne_prime % 48271; it is important that modulus < quotient according to Knuth
            unsigned int     non_mers_prime    = 2147483399;
            unsigned short    seed2             = 40692;
            unsigned short    quot2             = 52774;
            unsigned short    modu2             = 3791;
            
            unsigned int*     temp_uint_ptr;
            float*            temp_float_ptr;
            
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


                /*
                // Garbage Collection
                while(*temp_float_ptr != NULL)
                    delete (temp_float_ptr);
                while(*temp_uint_ptr != NULL)
                    delete (temp_uint_ptr);
                */
            
                // return a unique hash value
                return hash3;
            } 
        }
        


/////////////////////////////////////////////////////////////////////////
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
            Serial.println(F("-- Thermistor Test --"));
            Serial.print(F("\n"));
            IR_Thermistor_Temperature();
            //take_IR_Image_8x8();
            take_IR_Image_1x64();
            delay(1000);
            return;
        }



//////////////////////////////////////////////////////////////////////
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
            pinMode(13, OUTPUT);  // Multicolored_LED | On-board ItsyBitsy  | Pin 13
            
            // set speed of Stepper Motor (60 rpm)
            stepperMotor.setSpeed(120);

            // wait 3 sec
            delay(3000);

            // Greeting
            Serial.println(F("Welcome to the ARGO IR Instrument!"));
            Serial.print(F("\n")); // print a newline
            
            return;
        }


        ////////////////
        // Main: Loop //
        ////////////////
        void loop()   // the loop function runs over and over again forever   
        {
            // Define local variables:
            char menuChoice;

            Serial.flush();
            
            while (Serial.available() > 0)   // Constantly checks to see if anything has been sent over the USB Connection and if it needs to be processed
            {

            
                // Present Menu Options
                Serial.println(F("\t* * * Main Menu * * *"));
                Serial.println(F("\t1.) Open Shutter"));
                Serial.println(F("\t2.) Close Shutter"));
                Serial.println(F("\t3.) Motor-Synced LED Test"));
                Serial.println(F("\t4.) IR Camera Test"));
                Serial.print(F("\n")); // print a newline

                // Main Menu
                    menuChoice = Serial.read(); // Reads a single letter
                    
                    if(menuChoice >= 48 && menuChoice <= 57) // if a valid menuChoice ASCII 0-9 was selected
                    {
                        Serial.print(F("Menu Choice \""));
                        Serial.print(menuChoice);
                        Serial.println(F("\" was selected"));
                    }
                    else
                    {
                        Serial.println(F("\n* * * ERROR: Invalid Input! * * *"));
                        Serial.println(F("* * *   Please Try Again    * * *\n"));
                    }
                    
                    switch(menuChoice)
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
    
                    Serial.println(F("Returning to Main Menu"));
                    Serial.println(F("Please select one of the options listed:"));                   
                    Serial.print(F("\n")); // print a newline
                    Serial.flush(); // Clears the input
            }
        }
