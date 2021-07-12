#include "ARGO_STEPPER_MOTOR.h"

ArgoStepper::ArgoStepper(steps_per_rev, Ain1, Ain2, Bin2, Bin1)
{
    // Use 'this->' to make the difference between the
    // 'Ain1' attribute of the class and the 
    // local variable 'Ain1' created from the parameter.
    this->steps_per_rev = steps_per_rev;
    this->Ain1 = Ain1;
    this->Ain2 = Ain2;
    this->Bin2 = Bin2;
    this->Bin1 = Bin1;
}

//////////////////////////////
// Subroutine: Open_Shutter //
//////////////////////////////
void ArgoStepper::openShutter()
{
    stepperMotor.step(shutter_rotation_steps);   // Clockwise <CW> | +360/1.8[*] = +150[*] (+0.45 [Rev]) 
    Serial.println("Shutter Opened! <Clockwise, CW>");
    return;
}


///////////////////////////////
// Subroutine: Close_Shutter //
///////////////////////////////
void ArgoStepper::closeShutter()
{
    stepperMotor.step(-shutter_rotation_steps);  // Counter-Clockwise <CCW> | -360/1.8[*] = -150[*] (-0.45 [Rev]) (see above commentary)
    Serial.println("Shutter Closed! <Counter-Clockwise, CCW>");
    return;
}
