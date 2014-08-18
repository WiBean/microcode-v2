#pragma once

#include "UniformlySpacedOutputSortedLookupTable.h"
#include <stdint.h>

class Thermistor : public UniformlySpacedOutputSortedLookupTable<uint16_t, uint16_t, float, float>
{
public:
    Thermistor();
    float getTemperature(uint16_t reading);
    // LUT for thermistor

private:
	static uint16_t const INPUT_SIZE = 331;
    static uint16_t const INPUTS[INPUT_SIZE]; // non-integral init later
protected:
	virtual uint16_t getArrSize() {
        return INPUT_SIZE;
    };
    virtual uint16_t const* getArr() {
        return INPUTS;
    };

};
