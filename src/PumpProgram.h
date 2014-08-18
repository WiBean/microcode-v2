#pragma once

#include <algorithm>
#include "stdint.h"

template <int N>
class PumpProgram
{
public:
  typedef uint32_t PUMP_TIME_TYPE;
  static const uint32_t PUMP_STEPS = N;
  // whats the longest time a single program can be 'on' or 'off'
  // this should prevent anyone from pumping for days at a time
  static const uint32_t MAX_STATE_DURATION_IN_MS = 60000;
  static const uint32_t MAX_STATE_DOUBLE = 2*MAX_STATE_DURATION_IN_MS;

  PumpProgram() {
    // initialize arrays to 0
    memset(mOnTimes, 0, N*sizeof(int));
    memset(mOffTimes, 0, N*sizeof(int));
    mLastCounter = 0;
  }

  void validateProgram(uint16_t numel, uint32_t onTimes[], uint32_t offTimes[]) {
    // ensure the first off time is <= max state time
    // NOTE: this approach with the math handles rollover appropriately
    offTimes[0] = std::min(offTimes[0], onTimes[0]+MAX_STATE_DURATION_IN_MS);
    // now that we've checked the first offTime relative to the on time...
    // no single state can be greater than MAX_STATE, so we can check for
    // double that on each array individually
    clampArray(numel, onTimes);
    clampArray(numel, offTimes);
  }

  void clampArray(uint16_t numel, uint32_t input[]) {
    for(uint32_t k=1;k<numel;++k) {
      // NOTE: this approach with the math handles rollover appropriately
      input[k] = std::min(input[k], input[k-1]+MAX_STATE_DOUBLE);
    }
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

  bool isPumpingAt(uint32_t currentTimeInMillis) {
    for(uint32_t k=0;k<N;++k) {
      bool const offGood = (currentTimeInMillis < mOffTimes[k]);
      // if needed, mofiy logic when going through an overflow
      if( mOffTimes[k] < mOnTimes[k] && offGood ) {
        return true;
      }
      if( (currentTimeInMillis >= mOnTimes[k]) && offGood ) {
        return true;
      }
    }
    return false;
  }

  bool isValveOpenAt(uint32_t currentTimeInMillis) {
    return ( (currentTimeInMillis >= mOnTimes[0]) &&
              (currentTimeInMillis <= mOffTimes[PUMP_STEPS-1]) );
  }

private:
  // the Spark millis() function is a 32-bit unsigned counter
  // stored as when to turn on and when to turn off based on an
  // ever increasing millis counter.  We store the previous value
  // to detect and account for overflows
  uint32_t mOnTimes[N];
  uint32_t mOffTimes[N];
  uint32_t mLastCounter;

};
