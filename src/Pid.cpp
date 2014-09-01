#include "Pid.h"

#include <limits>
#include <algorithm>

// ***********
// DEBUG (keep this block last)
#define SERIAL_DEBUG
// ***********
#if defined(SERIAL_DEBUG) && defined(SPARK)
    #include "application.h"
#endif

PID::PID()
{
    mError_integral = 0;
    mError_previous = 0;
    mDerivative = 0;
    mError_integral_max = (std::numeric_limits<float>::max)();
    mError_integral_min = (std::numeric_limits<float>::lowest)();
    mOutput_min = (std::numeric_limits<float>::lowest)();
    mOutput_max = (std::numeric_limits<float>::max)();

    // by default just be a P controller
    mCoeff_proportional = 1;
    mCoeff_integral = 0;
    mCoeff_derivative = 0;
    mOutput = 0;
}


float PID::updateError(float error)
{
    //error = setpoint - measured_value
    mError_integral = (std::max)((std::min)(mError_integral + error, mError_integral_max), mError_integral_min);
    
    mDerivative = (error - mError_previous);

    float const unboundedOutput = mCoeff_proportional*error + mCoeff_integral*mError_integral + mCoeff_derivative*mDerivative;
    mOutput = (std::max)((std::min)(unboundedOutput, mOutput_max), mOutput_min);
#if defined(SERIAL_DEBUG) && defined(SPARK)
    Serial.println(String(error,1) + "," + String(mCoeff_proportional*error,1) + ","
    + String(mCoeff_integral*mError_integral, 1) + "," + String(mCoeff_derivative*mDerivative,1) + ","
    + String(mOutput,1));
#endif
    
    mError_previous = error;
    
    return mOutput;
}


void PID::setOutputBounds(float minOutput, float maxOutput)
{
    mOutput_min = minOutput;
    mOutput_max = maxOutput;
};
void PID::setIntegralBounds(float minValue, float maxValue)
{
    mError_integral_min = minValue;
    mError_integral_max = maxValue;
};
void PID::setCoefficientProportional(float coeff)
{
    mCoeff_proportional = coeff;
};
void PID::setCoefficientIntegral(float coeff)
{
    mCoeff_integral = coeff;
};
void PID::setCoefficientDerivative(float coeff)
{
    mCoeff_derivative = coeff;
}
float PID::getOutput() const {
    return mOutput;
}

void PID::resetState() {
    mError_previous = 0;
    mError_integral = 0;
    mOutput = 0;
}

float PID::getCombinedIntegralTerm() const
{
    return mCoeff_integral*mError_integral;
};
float PID::getCombinedProportionalTerm() const
{
    return mCoeff_proportional*mError_previous;
};
float PID::getCombinedDerivativeTerm() const
{
    return mCoeff_derivative*mDerivative;
};