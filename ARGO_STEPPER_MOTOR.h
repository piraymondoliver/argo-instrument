#ifndef ARGO_STEPPER_MOTOR_H
#define ARGO_STEPPER_MOTOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>

/////////////////                                           
// Define Pins //
/////////////////

    // Motor Stepper Inputs
    #define Ain1                    7;    // Green Wire   from Stepper Motor AIN1 to ItsyBitsy Pin 7
    #define Ain2                    9;    // Black Wire   from Stepper Motor AIN2 to ItsyBitsy Pin 9
    #define Bin2                    10;   // Blue wire    from Stepper Motor BIN2 to ItsyBitsy Pin 10
    #define Bin1                    11;   // Red wire     from Stepper Motor BIN1 to ItsyBitsy Pin 11


//////////////////////////////////////////
// Define Motor Constants and Variables //
//////////////////////////////////////////
        
    // Define Motor Steps Per Revolution
    #define steps_per_rev           200;   // motor steps per revolution (from motor info)
    #define shutter_rotation_angle  150;   // angle of required to open or close shutter
    #define deg_per_step            1.8;   // degrees per step
        
    double shutter_rotation_steps = ( shutter_rotation_angle / deg_per_step );  // 200 steps  = 360* degrees
                                                                                // 1 step     = 1.8* deg
                                                                                // we need 150* of rotation
                                                                                // =: x steps  = 150deg
                                                                                // => x steps  = 150deg * (1 step / 1.8deg)
                                                                                // => x steps  = (150/1.8) to fully open/close

class ArgoStepper
{
    private:
        // Stepper Motor
        Stepper _ArgoStepper = Stepper(steps_per_rev, Ain1, Ain2, Bin2, Bin1);  // : Motor Stepper Inputs; see above pins in "Define Motor Constants and Variables"
    public:
        ArgoStepper(steps_per_rev, Ain1, Ain2, Bin2, Bin1)
        void openShutter();
        void closeShutter();
};

#endif
