#pragma once

#include "SortedLookupTable.h"
#include <stdint.h>

class Thermistor : public SortedLookupTable<uint16_t>
{
public:

    Thermistor();

    float getTemperature(uint16_t reading);
    // LUT for thermistor


private:
    static INDEX_TYPE const INPUT_SIZE = 331;
    static uint16_t const INPUTS[INPUT_SIZE]; // non-integral init later

    static float const OUTPUT_MIN; // non-integral init later
    static float const OUTPUT_TABLE_DELTA; // non-integral init later
    static float const OUTPUT_TABLE_DELTA_FLOAT; // non-integral init later

protected:
    virtual INDEX_TYPE getArrSize() {
        return INPUT_SIZE;
    };
    virtual uint16_t const* getArr() {
        return INPUTS;
    };


private:

    // 0 is lower, 1 is upper, everything else is inbetween
    static float computeOutputValue(INDEX_TYPE baseStep, float interp);
    // returns how far the value lies inbetween two bounding values.  Assumes the value <= high and value >= low
    static float computeInterpFactor(INDEX_TYPE low, INDEX_TYPE high, INDEX_TYPE value);

};
