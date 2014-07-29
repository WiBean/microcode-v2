#include "HeatingSM.h"

#include <algorithm>
#include <cmath>

float const HeatingSM::RELAY_CUTOFF_OFFSET_CELSIUS = 15;
float const HeatingSM::RELAY_HYSTERESIS_MARGIN_CELSIUS = 4;
float const HeatingSM::MAX_GOAL_TEMP = 120;
float const HeatingSM::PULSE_HYSTERESIS_MARGIN_CELSIUS = 2;

HeatingSM::HeatingSM()
{
  mCurrentTempInCelsius = 0;
  mGoalTempInCelsius = 0;
  mState = HIBERNATE;

  // limits to setup hysteresis between full and pulse state
  mRelayHighCutoffCelsius = 0;
  mRelayLowKickInCelsius = 0;
  mCycleLengthInMilliseconds = 1;
  mThyristorOn = false;

  mThyristorHeatingBlinkWidthInCycles = 0;
  mThyristorHeatingBlinkOffWidthInCycles = 1;
  mThyristorHeatingDwellWidthInCycles = 1;
  mCoolingBlinkWidthInCycles = 0;
  mCoolingDwellWidthInCycles = 1;

  mRunStep = 0;
}

bool HeatingSM::setCycleLengthInMilliseconds(uint16_t lengthInMs)
{
  mCycleLengthInMilliseconds = static_cast<float>(lengthInMs);

  mThyristorHeatingBlinkWidthInCycles = std::max(
          ceil(THYRISTOR_HEATING_BLINK_WIDTH_IN_MS/mCycleLengthInMilliseconds),
          1.);
  mThyristorHeatingBlinkOffWidthInCycles = std::max(
          ceil(THYRISTOR_HEATING_BLINK_OFF_WIDTH_IN_MS/mCycleLengthInMilliseconds),
          1.);
  mCoolingBlinkWidthInCycles = std::max(
          ceil(COOLING_BLINK_WIDTH_IN_MS/mCycleLengthInMilliseconds),
          1.);
  mCoolingDwellWidthInCycles = std::max(
          ceil(COOLING_DWELL_WIDTH_IN_MS/mCycleLengthInMilliseconds),
          1.);
  return true;
};


void HeatingSM::enableHeating(bool enable)
{
  if( enable ) {
    mState = DETERMINE_STATE;
  }
  else {
    mState = HIBERNATE;
  }
}

bool HeatingSM::setGoalTemperature(float temperatureInCelsius)
{
  if( temperatureInCelsius > MAX_GOAL_TEMP ) {
    return false;
  }
  mGoalTempInCelsius = temperatureInCelsius;

  mRelayHighCutoffCelsius = mGoalTempInCelsius - RELAY_CUTOFF_OFFSET_CELSIUS;
  mRelayLowKickInCelsius = mRelayHighCutoffCelsius - RELAY_HYSTERESIS_MARGIN_CELSIUS;
  // if heating, go back to searching state
  if( mState != HIBERNATE ) {
    mState = DETERMINE_STATE;
  }
  return true;
};

bool HeatingSM::runRelay()
{
  return (mState == HEATING_RELAY);
};


bool HeatingSM::runThyristor()
{
  return mThyristorOn;
};


/**
 * MAIN METHOD FOR UPDATING STATE
 *
 */
bool HeatingSM::updateCurrentTemp(float temperatureInCelsius)
{
  mCurrentTempInCelsius = temperatureInCelsius;
  switch( mState ) {
  case DETERMINE_STATE:
    if( mCurrentTempInCelsius < mRelayHighCutoffCelsius ) {
      mState = HEATING_RELAY;
    }
    else if( mCurrentTempInCelsius < mGoalTempInCelsius ) {
      mState = HEATING_PULSE;
    }
    else {
      mState = COOLING;
    }
    break;
  case HEATING_RELAY:
    if( mCurrentTempInCelsius < mRelayHighCutoffCelsius ) {
      // do nothing
    }
    else {
      mState = HEATING_PULSE;
    }
    break;
  case HEATING_PULSE:
    if( mCurrentTempInCelsius > mGoalTempInCelsius) {
      mRunStep = 0; // reset loop state;
      mState = COOLING;
    }
    else if( mCurrentTempInCelsius < mRelayLowKickInCelsius ) {
      mRunStep = 0; // reset loop state
      mState = HEATING_RELAY;
    }
    else {
      // compute dwell interval based on temperature
      if( mRunStep == 0 ) {
        mThyristorHeatingDwellWidthInCycles = computeThyristorDwellTimeInCycles();
      }
      // thyristor does 3 blinks, and then a dwell
      //blink on
      if( mRunStep < mThyristorHeatingBlinkWidthInCycles ) {
        mThyristorOn = true;
      }
      // blink valley
      else if( mRunStep <
                  1*mThyristorHeatingBlinkWidthInCycles+
                  1*mThyristorHeatingBlinkOffWidthInCycles ) {
        mThyristorOn = false;
      }
      // blink on
      else if( mRunStep <
                  2*mThyristorHeatingBlinkWidthInCycles+
                  1*mThyristorHeatingBlinkOffWidthInCycles ) {
        mThyristorOn = true;
      }
      // blink valley
      else if( mRunStep <
                  2*mThyristorHeatingBlinkWidthInCycles+
                  2*mThyristorHeatingBlinkOffWidthInCycles ) {
        mThyristorOn = false;
      }
      // blink on
      else if( mRunStep <
                  3*mThyristorHeatingBlinkWidthInCycles+
                  2*mThyristorHeatingBlinkOffWidthInCycles ) {
        mThyristorOn = true;
      }
      // dwell
      else if( mRunStep <
                  3*mThyristorHeatingBlinkWidthInCycles+
                  2*mThyristorHeatingBlinkOffWidthInCycles+
                  1*mThyristorHeatingDwellWidthInCycles ) {
        mThyristorOn = false;
      }
      // do it again!
      else {
        mRunStep = 0;
        break;
      }
      ++mRunStep;
    }
    break;
  case COOLING:
    if( mCurrentTempInCelsius < mGoalTempInCelsius ) {
      mState = HEATING_PULSE;
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
  }
  return true;
};


uint16_t HeatingSM::computeThyristorDwellTimeInCycles()
{
  // for now, use a fixed 400ms
  return std::max(
          ceil(500.f / mCycleLengthInMilliseconds),
          1.);
}
