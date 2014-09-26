#pragma once

/**
 * This is a virtual heating controller.  It assumes the caller/user is operating on a fixed interval
 * hardware timer which takes an action every N milliseconds.
*/

#include "Pid.h"

#include "stdint.h"


class HeatingSMPID {

public:
  enum State {
    HIBERNATE,
    DETERMINE_STATE,
    HEATING_PID,
    COOLING,
    PUMPING
  };

  static float const MAX_GOAL_TEMP;
  
private:
  static uint16_t const COOLING_BLINK_WIDTH_IN_MS = 100;
  static uint16_t const COOLING_DWELL_WIDTH_IN_MS = 7000;

public:

    HeatingSMPID();

    // true enables the heat loop, false goes into hibernation until awakened
    void enableHeating(bool enable);
    bool isHeating() const;
    State getState() const;
    // update the goal temperature and check validity
    bool updateGoalTemperatureInCelsius(float temperatureInCelsius);
    // force the user to set cycleLength in ms even though it's stored interally as float
    bool setCycleLengthInMilliseconds(uint16_t lengthInMs);
    // updates temperature and also then appropriately updates state
    bool updateCurrentTemp(float temperatureInCelsius);
    // update the goal temperature and check validity
    bool setGoalTemperature(float temperatureInCelsius);
    float getGoalTemperature() const;

    // returns true if heating cycle activate the relay
    bool runRelay() const;
    // return true if heating cycle should activate thyristor, also decrements cyclesToPulse counter (assumes caller takes action!)
    bool runThyristor() const;
    // inform the state manager that we are pumping
    void informOfPumping(bool pumpingNow);
    // allow outsiders to get at the PID parameters
    PID const getPID() const;

private:
    uint16_t computeThyristorRunInCycles(float pidOutput);

private:
    State mState;
    float mGoalTempInCelsius;
    float mCurrentTempInCelsius;

    float mCycleLengthInMilliseconds; // keep this a float so division is good!

    uint16_t mRunStep;
    uint16_t mRunStepsInCycle;

    bool mThyristorOn;

    uint16_t mThyristorOnForCycles;

    uint16_t mCoolingBlinkWidthInCycles;
    uint16_t mCoolingDwellWidthInCycles;

    PID mPid;
};
