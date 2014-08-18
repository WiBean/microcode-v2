#pragma once

#include <utility>
#include "stdint.h"

// virtual base class for LookupTables which are sorted.
// Implementation must make a derived class which defines the data
// INDEX_TYPE is the value type which is used to index into the LUT
// VALUE_TYPE is the value type used by the data in the LUT
// INTERP_PREC is float/double or the value type used when interpolating
template <typename VALUE_TYPE, typename INDEX_TYPE=uint32_t, typename INTERP_PREC=float>
class SortedLookupTable {

protected:
    typedef std::pair<INDEX_TYPE,INDEX_TYPE> IndexPair;
    // user must implement these to point at the data in the base class
    virtual VALUE_TYPE const* getArr() =0;
    virtual INDEX_TYPE getArrSize()=0;

    INDEX_TYPE mid(INDEX_TYPE const low, INDEX_TYPE const high)
    {
        return low + (high-low)/2;
    }

   IndexPair linearSearchBounds(VALUE_TYPE input)
    {
        auto const*const arr = getArr();
        // our array is sorted in increasing order
        if( input <= arr[0] ) {
            return IndexPair(0,0);
        }
        for(INDEX_TYPE k=1;k<getArrSize();++k)
        {
            if( arr[k] == input ) {
                return IndexPair(k,k);
            }
            // we know it's not equals, so don't check
            else if( input < arr[k] ) {
                return IndexPair(k-1,k);
            }
        }
        return IndexPair(getArrSize()-1,getArrSize()-1);
    }

    IndexPair binarySearchBounds(VALUE_TYPE const input)
    {
        auto const*const arr = getArr();
        // if the value is at or above the 0 index value, our lowValue check will catch it
        INDEX_TYPE searchIndexMin = 0;
        INDEX_TYPE searchIndexMax = getArrSize()-1;
		// allow the branch predictor to take the best path based on the table and the query types
		if (input <= arr[0]) {
			return IndexPair(0, 0);
		}
		else if (input >= arr[searchIndexMax]) {
			return IndexPair(searchIndexMax, searchIndexMax);
		}
		else {
			while (searchIndexMax > (searchIndexMin + 1)) {
				// calculate the midpoint to search next
				// NOTE: this way of calculating the mid ensures our midIndex will never == INPUT_SIZE-1
				// this is important because it ensures out index+1 check below will never index OOB on the high side
				INDEX_TYPE const midIndex = mid(searchIndexMin, searchIndexMax);
				VALUE_TYPE const value = arr[midIndex];
				if (value == input) {
					return IndexPair(midIndex, midIndex);
				}
				else {
					if (input > value) {
						searchIndexMin = midIndex;
					}
					// we know it's not equals because we checked above
					else {
						searchIndexMax = midIndex;
					}
				}
			}
		}
		return IndexPair(searchIndexMin, searchIndexMax);
    }

	
	// returns how far the value lies in-between two bounding values.  
	// Assumes the value <= high and value >= low
	static INTERP_PREC computeInterpFactor(VALUE_TYPE low, VALUE_TYPE high, VALUE_TYPE value)
	{
	  // don't get bit by div-by-0
	  if( high == low ) {
		return low;
	  }
	  INTERP_PREC const inputDelta = high - low;
	  INTERP_PREC const valueDelta = value - low;
	  INTERP_PREC const interpFactor = valueDelta / inputDelta;
	  #ifdef SERIAL_DEBUG
		Serial.print("cIF factor: ");
		Serial.println(interpFactor);
	  #endif
	  return interpFactor;
	}
};
