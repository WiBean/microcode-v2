#pragma once

/**
 * This is a virtual heating controller.  It assumes the caller/user is operating on a fixed interval
 * hardware timer which takes an action every N milliseconds.
*/

#include "stdint.h"

class HeatingSM {

private:
  enum State {
      DETERMINE_STATE,
      HEATING_RELAY,
      HEATING_PULSE,
      COOLING,
      HIBERNATE
  };
  static float const RELAY_CUTOFF_OFFSET_CELSIUS;
  static float const RELAY_HYSTERESIS_MARGIN_CELSIUS;
  static float const MAX_GOAL_TEMP;
  static float const PULSE_HYSTERESIS_MARGIN_CELSIUS;

  static uint16_t const THYRISTOR_HEATING_BLINK_WIDTH_IN_MS = 100;
  static uint16_t const THYRISTOR_HEATING_BLINK_OFF_WIDTH_IN_MS = 100;
  static uint16_t const COOLING_BLINK_WIDTH_IN_MS = 100;
  static uint16_t const COOLING_DWELL_WIDTH_IN_MS = 7000;

public:

    HeatingSM();

    // true enables the heat loop, false goes into hibernation until awakened
    void enableHeating(bool enable);
    bool isHeating();
    // update the goal temperature and check validity
    bool updateGoalTemperatureInCelsius(float temperatureInCelsius);
    // force the user to set cycleLength in ms even though it's stored interally as float
    bool setCycleLengthInMilliseconds(uint16_t lengthInMs);
    // updates temperature and also then appropriately updates state
    bool updateCurrentTemp(float temperatureInCelsius);
    // update the goal temperature and check validity
    bool setGoalTemperature(float temperatureInCelsius);
    uint16_t computeThyristorDwellTimeInCycles();

    // returns true if heating cycle activate the relay
    bool runRelay();
    // return true if heating cycle should activate thyristor, also decrements cyclesToPulse counter (assumes caller takes action!)
    bool runThyristor();

private:
    State mState;
    float mGoalTempInCelsius;
    float mCurrentTempInCelsius;
    // limits to setup hysteresis between full and pulse state
    float mRelayHighCutoffCelsius;
    float mRelayLowKickInCelsius;

    float mCycleLengthInMilliseconds; // keep this a float so division is good!
    uint16_t mRunStep;
    bool mThyristorOn;

    uint16_t mThyristorHeatingBlinkWidthInCycles;
    uint16_t mThyristorHeatingBlinkOffWidthInCycles;
    uint16_t mThyristorHeatingDwellWidthInCycles;

    uint16_t mCoolingBlinkWidthInCycles;
    uint16_t mCoolingDwellWidthInCycles;
};
