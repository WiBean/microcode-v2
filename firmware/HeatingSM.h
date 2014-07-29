#pragma once

/**
 * This is a virtual heating controller.  It assumes the caller/user is operating on a fixed interval
 * hardware timer which takes an action every N milliseconds.
*/

class HeatingSM {

enum State {
    SLEEPING,
    HEATING_RELAY,
    HEATING_PULSE
};

public:

    HeatingSM();

    bool setCycleLengthInMilliseconds(int lengthInMs);
    // updates temperature and also then appropriately updates state
    bool updateCurrentTemp(float temperatureInCelsius);

    // returns true if heating cycle activate the relay
    bool runRelay();
    // return true if heating cycle should activate thyristor, also decrements cyclesToPulse counter (assumes caller takes action!)
    bool runThyristor();

private:
    State mState;
    float mGoalTempInCelsius;
    float mCurrentTempInCelsius;

    // limits to setup hysteresis between full and pulse state
    float mStateFullUpperTemperatureLimit;
    float mStatePulseLowerTemperatureLimit;

    int mCycleLengthInMilliseconds;
    int mCyclesToPulse;

};
