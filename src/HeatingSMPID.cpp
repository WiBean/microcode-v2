#include "HeatingSMPID.h"

#include <algorithm>
    
float const HeatingSMPID::MAX_GOAL_TEMP = 120;

HeatingSMPID::HeatingSMPID()
{
  mCurrentTempInCelsius = 0;
  mGoalTempInCelsius = 0;
  mState = HIBERNATE;

  // limits to setup hysteresis between full and pulse state
  mCycleLengthInMilliseconds = 1;
  mThyristorOn = false;

  mCoolingBlinkWidthInCycles = 0;
  mCoolingDwellWidthInCycles = 1;
  mThyristorOnForCycles = 0;

  mRunStep = 0;

  // initialize the PID (TUNE COEFFICIENTS HERE)
  mPid.setCoefficientProportional(-17);
  mPid.setCoefficientDerivative(-3);
  mPid.setCoefficientIntegral(-0.2);
  // PID output ranges from 0 -> 1000 to work like a duty cycle
  mPid.setOutputBounds(0, 1000);
  // THIS IS ADJUSTED LATER in setGoalTemperature
  mPid.setIntegralBounds(-1, 1);
}

bool HeatingSMPID::setCycleLengthInMilliseconds(uint16_t lengthInMs)
{
  mCycleLengthInMilliseconds = static_cast<float>(lengthInMs);
  mRunStepsInCycle = std::ceil(1000.f / mCycleLengthInMilliseconds);

  mCoolingBlinkWidthInCycles = std::max(
          ceilf(COOLING_BLINK_WIDTH_IN_MS/mCycleLengthInMilliseconds),
          1.f);
  mCoolingDwellWidthInCycles = std::max(
          ceilf(COOLING_DWELL_WIDTH_IN_MS/mCycleLengthInMilliseconds),
          1.f);
  return true;
};


void HeatingSMPID::enableHeating(bool enable)
{
    if( enable ) {
        mPid.resetState();
        mState = DETERMINE_STATE;
    }
    else {
        mState = HIBERNATE;
    }
}


bool HeatingSMPID::isHeating()
{
  return (mState != HIBERNATE);
}

bool HeatingSMPID::setGoalTemperature(float temperatureInCelsius)
{
  if( temperatureInCelsius > MAX_GOAL_TEMP ) {
    return false;
  }
  mGoalTempInCelsius = temperatureInCelsius;
  // keep the PID controller bounded
  mPid.setIntegralBounds(-10*mGoalTempInCelsius, 10*mGoalTempInCelsius);

  // if heating, go back to searching state
  if( isHeating() ) {
    mState = DETERMINE_STATE;
  }
  return true;
};

bool HeatingSMPID::runRelay()
{
    return ( (mState == PUMPING) ||
            ((mState==HEATING_PID) && (mPid.getOutput() >= 1000.f))
           );
};


bool HeatingSMPID::runThyristor()
{
  return ((mState == PUMPING) || mThyristorOn);
};


/**
 * MAIN METHOD FOR UPDATING STATE
 *
 */
bool HeatingSMPID::updateCurrentTemp(float const temperatureInCelsius)
{
  mCurrentTempInCelsius = temperatureInCelsius;
  mPid.updateError(mCurrentTempInCelsius - mGoalTempInCelsius);

  switch( mState ) {
  case DETERMINE_STATE:
    if( mCurrentTempInCelsius < mGoalTempInCelsius ) {
        mState = HEATING_PID;
    }
    else {
      mState = COOLING;
    }
    break;
  case HEATING_PID:
    if( mCurrentTempInCelsius > mGoalTempInCelsius) {
      mRunStep = 0; // reset loop state;
      mState = COOLING;
    }
    else {
      // compute dwell interval based on temperature
      if( mRunStep == 0 ) {
        if( mPid.getOutput() >= 1000.f ) {
            mThyristorOnForCycles = 0;
        }
        else {
            mThyristorOnForCycles = computeThyristorRunInCycles(mPid.getOutput());
        }
      }
      //blink on
      mThyristorOn = (mRunStep < mThyristorOnForCycles);
      mRunStep = (mRunStep + 1) % mRunStepsInCycle;
    }
    break;
  case COOLING:
    if( (mRunStep == 0) && (mCurrentTempInCelsius < mGoalTempInCelsius) ) {
      mState = HEATING_PID;
      mRunStep = 0;
    }
    // cooling does one blink, then a long dwell
    //blink on
    else {
      if( mRunStep < mCoolingBlinkWidthInCycles ) {
      mThyristorOn = true;
      }
      // blink valley
      else if( mRunStep <
                  1*mCoolingBlinkWidthInCycles+
                  1*mCoolingDwellWidthInCycles ) {
        mThyristorOn = false;
      }
      else {
        // start over!
        mRunStep = 0;
        break;
      }
      ++mRunStep;
    }
    break;
  case HIBERNATE:
    mThyristorOn = false;
    // do nothing
    break;
  case PUMPING:
    // the relay will handle heating now
    mThyristorOn = false;
    break;
  default:
    // we shouldn't be here
    mState = HIBERNATE;
    break;
  }
  return true;
};


uint16_t HeatingSMPID::computeThyristorRunInCycles(float pidOutput)
{
    // we are using a 20hz cycle, pidOutput is capped at [0,1000]
    // looks like with the gcc-arm compiler we only get round with a double
    return std::floor((pidOutput + 50.f) / 50.f);
};


void HeatingSMPID::informOfPumping(bool pumpingNow)
{
  if( mState != HIBERNATE ) {
    if( pumpingNow ) {
      mState = PUMPING;
    }
    else {
      mState = DETERMINE_STATE;
    }
  }
};

HeatingSMPID::State HeatingSMPID::getState() {
  return mState;
};

PID const HeatingSMPID::getPID()
{
    return mPid;
}