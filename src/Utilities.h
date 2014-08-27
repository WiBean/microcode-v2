#pragma once
/**
 ************
 * UTILITY
 ************
 */

#include <cstdint>
// only include if we are really on the ARM system
#ifdef SPARK
	#include "application.h"
#else
	// compile in the few things we have cross platform
	#include "spark_wiring_string.h"
#endif

namespace wibean {
    namespace utils {
        String boolToString(bool input);
        String floatToString(float input);
        // returns the value taken off the string, as well as the next position
        // returns position after value converted OR -1 if character invalid
        int takeNext(String const& command, uint16_t const start, int & outValue);

      
        template <typename T>
        void printArray(std::uint16_t numel, T const*const array) {
            unsigned char const DEC_BASE = static_cast<unsigned char>(DEC);
            Serial.print('[');
            for (std::uint16_t k = 0; k<numel - 1; ++k) {
                Serial.print(array[k],DEC_BASE);
                Serial.print(", ");
            }
            Serial.print(array[numel-1],DEC_BASE);
            Serial.println("]");
        }
        template <>
        void printArray<float>(std::uint16_t numel, float const*const array);
    }
}