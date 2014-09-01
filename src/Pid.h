#pragma once
/**
 ************
 * PID Controller
 ************
 * Output ranges from min/max.
 * Assumes the update function is called on a regular
 * interval controlled by the caller.  Hence all dt terms
 * are assumed to be 1.
 */

class PID
{
public:
    PID();

    // call this function regularly
    float updateError(float error);
    float getOutput() const;
    float getCombinedIntegralTerm() const;
    float getCombinedProportionalTerm() const;
    float getCombinedDerivativeTerm() const;

    void setOutputBounds(float minOutput, float maxOutput);
    void setIntegralBounds(float minValue, float maxValue);
    void setCoefficientProportional(float coeff);
    void setCoefficientIntegral(float coeff);
    void setCoefficientDerivative(float coeff);

    void resetState();

private:
    float mCoeff_proportional;
    float mCoeff_integral;
    float mCoeff_derivative;

    float mError_previous;
    float mError_integral;
    float mError_integral_max;
    float mError_integral_min;
    float mDerivative;

    float mOutput_min;
    float mOutput_max;
    float mOutput;


};