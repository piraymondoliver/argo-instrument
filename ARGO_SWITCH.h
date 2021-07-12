#ifndef ARGO_SWITCH_H
#define ARGO_SWITCH_H

#include <Arduino.h>
#include <Wire.h>

/////////////////                                           
// Define Pins //
/////////////////

    // Switch Inputs
    #define limitSwitchOpen     A0;    // Purple Wire  from Top Board to ItsyBitsy Pin A0
    #define limitSwitchClosed   A1;    // Orange Wire  from Top Board to ItsyBitsy Pin A1


class ArgoSwitch
{
    private:
        bool _limitSwitchOpenState;
        bool _limitSwitchClosedState;
    public:
        ArgoSwitch argoLimitSwitchOpen(_limitSwitchOpenState)
        ArgoSwitch argoLimitSwitchOpen(_limitSwitchClosedState);
        
        void closeShutter();
};

#endif
