#include "HeatingSM.h"





HeatingSM::HeatingSM()
{
    mCurrentTempInCelsius = 0;
    mGoalTempInCelsius = 0;
    mCyclesToPulse = 0;
    mState = SLEEPING;
}


bool HeatingSM::setCycleLengthInMilliseconds(int lengthInMs)
{
    if( lengthInMs <= 0 ) {
        return false;
    }
    mCycleLengthInMilliseconds = lengthInMs;
    return true;
}


bool HeatingSM::updateCurrentTemp(float temperatureInCelsius)
{
    return true;
};


bool HeatingSM::runRelay()
{
    return true;
}


bool HeatingSM::runThyristor()
{
    return true;
}
