#include "Pid.h"

#include <gtest\gtest.h>


// ************
// INIT TEST FIXTURE 
// ***********
class PidControllerTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    };
    // virtual void TearDown() {}
    PID pid_;
};


// test wibean::utils::boolToString
TEST_F(PidControllerTest, ProportionalOnlyMode)
{
    //  on init output is 0
    EXPECT_EQ(pid_.getOutput(), 0);

    pid_.setCoefficientProportional(1);
    pid_.setCoefficientIntegral(0);
    pid_.setCoefficientDerivative(0);

    // Test proportional only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 10);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), -15);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -20);
};

TEST_F(PidControllerTest, DerivateOnlyMode)
{ 
    pid_.setCoefficientProportional(0);
    pid_.setCoefficientIntegral(0);
    pid_.setCoefficientDerivative(1);
    
    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 5);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), -25);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -5);
};

TEST_F(PidControllerTest, IntegralOnlyMode)
{
    pid_.setCoefficientProportional(0);
    pid_.setCoefficientIntegral(1);
    pid_.setCoefficientDerivative(0);

    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 15);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), 0);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -20);
};

TEST_F(PidControllerTest, ProportionalIntegralMode)
{
    pid_.setCoefficientProportional(1);
    pid_.setCoefficientIntegral(1);
    pid_.setCoefficientDerivative(0);

    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5 + 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 10 + 15);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), -15 + 0);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -20 + -20);
};

TEST_F(PidControllerTest, ProportionalDerivativeMode)
{
    pid_.setCoefficientProportional(1);
    pid_.setCoefficientIntegral(0);
    pid_.setCoefficientDerivative(1);

    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5 + 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 10 + 5);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), -15 + -25);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -20 + -5);
};

TEST_F(PidControllerTest, DerivativeIntegralMode)
{
    pid_.setCoefficientProportional(0);
    pid_.setCoefficientIntegral(1);
    pid_.setCoefficientDerivative(1);

    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5 + 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 5 + 15);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), -25 + 0);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -5 + -20);
};

TEST_F(PidControllerTest, ProportionalIntegralDerivativeMode)
{
    pid_.setCoefficientProportional(1);
    pid_.setCoefficientIntegral(1);
    pid_.setCoefficientDerivative(1);

    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5 + 5 + 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 10 + 15 + 5);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), -15 + 0 + -25);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), -20 + -20 + -5);
};

TEST_F(PidControllerTest, OutputLimits)
{
    pid_.setCoefficientProportional(1);
    pid_.setCoefficientIntegral(0);
    pid_.setCoefficientDerivative(0);

    pid_.setOutputBounds(0, 8);

    // Test derivative only mode
    pid_.updateError(5);
    EXPECT_EQ(pid_.getOutput(), 5);
    pid_.updateError(10);
    EXPECT_EQ(pid_.getOutput(), 8);
    pid_.updateError(-15);
    EXPECT_EQ(pid_.getOutput(), 0);
    pid_.updateError(-20);
    EXPECT_EQ(pid_.getOutput(), 0);
};