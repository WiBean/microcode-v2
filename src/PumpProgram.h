#pragma once

#include <algorithm>
#include "stdint.h"

// **********
// DEBUG
#undef SERIAL_DEBUG
//#define SERIAL_DEBUG
// **********
#ifdef SERIAL_DEBUG
    #include "application.h"
    unsigned char DEC_BASE = static_cast<unsigned char>(DEC);
    #include "Utilities.h"
#endif

template <uint16_t N>
class PumpProgram
{
public:
    typedef uint32_t PUMP_TIME_TYPE;
    static const uint16_t PUMP_STEPS = N;
    // whats the longest time a single program can be 'on' or 'off'
    // this should prevent anyone from pumping for days at a time
    static const PUMP_TIME_TYPE MAX_STATE_DURATION_IN_MS = 60000;
    static const PUMP_TIME_TYPE MAX_STATE_DOUBLE = 2*MAX_STATE_DURATION_IN_MS;

    PumpProgram() {
        mOnTimes = new PUMP_TIME_TYPE [N];
        mOffTimes = new PUMP_TIME_TYPE [N];
        for(uint16_t k=0;k<N;++k) {
            mOnTimes[k] = 0;
            mOffTimes[k] = 0;
        }
    }
    ~PumpProgram() {
        delete[] mOnTimes;
        delete[] mOffTimes;
    }

    static void validateProgram(uint16_t numel, uint32_t onTimes[], uint32_t offTimes[]) {
        // ensure the first off time is <= max state time
        // NOTE: this approach with the math handles rollover appropriately
        offTimes[0] = std::min(offTimes[0], onTimes[0]+MAX_STATE_DURATION_IN_MS);
        // now that we've checked the first offTime relative to the on time...
        // no single state can be greater than MAX_STATE, so we can check for
        // double that on each array individually
        clampArray(numel, onTimes);
        clampArray(numel, offTimes);
    }


    void setProgram(uint16_t numel, uint32_t onTimes[], uint32_t offTimes[]) {
        validateProgram(numel, onTimes, offTimes);
        uint32_t lastOff;
        for(uint16_t k=0;(k<N)&&(k<numel);++k) {
            mOnTimes[k] = onTimes[k];
            lastOff = mOffTimes[k] = offTimes[k];
        }
        // clamp any missing values
        for(uint16_t k=numel;k<N;++k) {
            mOnTimes[k] = lastOff;
            mOffTimes[k] = lastOff;
        }
    }

    bool isPumpingAt(uint32_t currentTimeInMillis) const {
        for(uint32_t k=0;k<N;++k) {
            bool const offGood = (currentTimeInMillis < mOffTimes[k]);
            // if needed, mofiy logic when going through an overflow
            if( (mOffTimes[k] < mOnTimes[k]) && offGood ) {
                return true;
            }
            if( (currentTimeInMillis >= mOnTimes[k]) && offGood ) {
                return true;
            }
        }
        return false;
    }

    bool isValveOpenAt(uint32_t const currentTimeInMillis) const{
        bool const result = ( (currentTimeInMillis >= mOnTimes[0]) &&
                                (currentTimeInMillis <= mOffTimes[PUMP_STEPS-1]) );
#ifdef SERIAL_DEBUG
    wibean::utils::printArray(PUMP_STEPS, mGuardArray);
    wibean::utils::printArray(PUMP_STEPS, mOnTimes);
    Serial.println("onT[0]: " + String(mOnTimes[0],DEC_BASE) + " curTime: " + String(currentTimeInMillis, DEC_BASE)
                    + " endT: " + String(mOffTimes[PUMP_STEPS-1], DEC_BASE) + " result: " + wibean::utils::boolToString(result));
#endif
        return result;
    }

private:
    static void clampArray(uint16_t numel, uint32_t input[]) {
        for (decltype(numel) k = 1; k<numel; ++k) {
            // NOTE: this approach with the math handles rollover appropriately
            input[k] = std::min(input[k], input[k - 1] + MAX_STATE_DOUBLE);
        }
    }

private:
    // the Spark millis() function is a 32-bit unsigned counter
    // stored as when to turn on and when to turn off based on an
    // ever increasing millis counter.  We store the previous value
    // to detect and account for overflows
    
    // When I used static arrays they were getting randomly over-written
    // so I'm thinking I had a stackoverflow
    PUMP_TIME_TYPE * mOnTimes;
    PUMP_TIME_TYPE * mOffTimes;

};
