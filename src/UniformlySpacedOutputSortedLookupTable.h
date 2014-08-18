#pragma once

#include "SortedLookupTable.h"
#include <stdint.h>
#include <algorithm>

// ASSUMES LUT IS SORTED

template <typename VALUE_TYPE, typename INDEX_TYPE=uint32_t, typename INTERP_PREC=float, typename OUTPUT_TYPE=float>
class UniformlySpacedOutputSortedLookupTable : public SortedLookupTable<VALUE_TYPE, INDEX_TYPE, INTERP_PREC>
{
public:

protected:
	// ASSIGN THESE IN THE DERIVED IMPLEMENTATION
	// the output value corresponding to the 0th position in the input array
	static OUTPUT_TYPE OUTPUT_MIN; // non-integral init later
	// the step in output corresponding to an integral jump in the input array
    static OUTPUT_TYPE OUTPUT_TABLE_DELTA; // non-integral init later
	// the same as the value above, but using the floating point typename so that interp can work
    static INTERP_PREC OUTPUT_TABLE_DELTA_INTERP_PREC; // non-integral init later
	
	// 0 is lower, 1 is upper, everything else is in-between
    INTERP_PREC computeOutputValue(INDEX_TYPE baseStep, INTERP_PREC interp) {
		// don't go under table min
		baseStep = (std::max)((INDEX_TYPE)0, baseStep);
		// if they want a value outside the LUT, return max LUT value
		baseStep = (std::min)(baseStep, this->getArrSize());

		// clamp the interp to safe values
		OUTPUT_TYPE retValue = OUTPUT_MIN + baseStep*OUTPUT_TABLE_DELTA;
		if( (interp < 0) || (interp > 1) ) {
			return retValue;
		}
		return retValue + OUTPUT_TABLE_DELTA_INTERP_PREC*interp;
	}
};
template <typename VALUE_TYPE, typename INDEX_TYPE, typename INTERP_PREC, typename OUTPUT_TYPE>
OUTPUT_TYPE UniformlySpacedOutputSortedLookupTable<VALUE_TYPE, INDEX_TYPE, INTERP_PREC, OUTPUT_TYPE>::OUTPUT_MIN;
template <typename VALUE_TYPE, typename INDEX_TYPE, typename INTERP_PREC, typename OUTPUT_TYPE>
OUTPUT_TYPE UniformlySpacedOutputSortedLookupTable<VALUE_TYPE, INDEX_TYPE, INTERP_PREC, OUTPUT_TYPE>::OUTPUT_TABLE_DELTA;
template <typename VALUE_TYPE, typename INDEX_TYPE, typename INTERP_PREC, typename OUTPUT_TYPE>
INTERP_PREC UniformlySpacedOutputSortedLookupTable<VALUE_TYPE, INDEX_TYPE, INTERP_PREC, OUTPUT_TYPE>::OUTPUT_TABLE_DELTA_INTERP_PREC;

