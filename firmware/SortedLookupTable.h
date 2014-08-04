#pragma once

#include <utility>
#include "stdint.h"

// virtual base class for LookupTables which are sorted.
// Implementation must make a derived class which defines the data

template <typename T>
class SortedLookupTable {

protected:
    typedef uint32_t INDEX_TYPE;
    typedef std::pair<INDEX_TYPE,INDEX_TYPE> IndexPair;
    // user must implement these to point at the data in the base class
    virtual T const* getArr() =0;
    virtual INDEX_TYPE getArrSize()=0;

    INDEX_TYPE mid(INDEX_TYPE const low, INDEX_TYPE const high)
    {
        return low + (high-low)/2;
    }

   IndexPair linearSearchBounds(T input)
    {
        T const*const arr = getArr();
        // our array is sorted in increasing order
        if( input <= arr[0] ) {
            return IndexPair(0,0);
        }
        for(int k=1;k<getArrSize();++k)
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

    IndexPair binarySearchBounds(T const input)
    {
        T const*const arr = getArr();
        // we start at 1 instead of zero to prevent OOB indexing on the low side
        // if the value is at or above the 0 index value, our lowValue check will catch it
        uint16_t searchIndexMin = 1;
        uint16_t searchIndexMax = getArrSize()-1;
        while( searchIndexMax > (searchIndexMin+1) ) {
            // calculate the midpoint to search next
            // NOTE: this way of calculating the mid ensures our midIndex will never == INPUT_SIZE-1
            // this is important because it ensures out index+1 check below will never index OOB on the high side
            uint16_t const midIndex = mid(searchIndexMin,searchIndexMax);
            uint16_t const value = arr[midIndex];
            if( value == input) {
                return IndexPair(midIndex,midIndex);
            }
            else {
                if( input > value ) {
                    uint16_t const highValue = arr[midIndex+1];
                    if( input <=  highValue ) {
                        return IndexPair(midIndex,midIndex+1);
                    }
                    else {
                        searchIndexMin = midIndex;
                    }
                }
                // we know it's not equals because we checked above
                else {
                    uint16_t const lowValue = arr[midIndex-1];
                    if( input >=  lowValue ) {
                        return IndexPair(midIndex-1,midIndex);
                    }
                    else {
                        searchIndexMax = midIndex;
                    }
                }
            }
        }
        // if we get here, we didn't find a match.  Return max or min value
        if( searchIndexMin == 0 ) {
            return IndexPair(0,0);
        }
        else {
            return IndexPair(getArrSize()-1,getArrSize()-1);
        }
    }
};
