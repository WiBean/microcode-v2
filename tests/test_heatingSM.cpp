#include "HeatingSM.h"

#include <gtest\gtest.h>


// ************
// INIT TEST FIXTURE 
// ***********
class HeatingSM_hibernateTest : public ::testing::Test {
protected:
	//virtual void SetUp() {}
	// virtual void TearDown() {}
    HeatingSM sm_;
};

class HeatingSM_determineStateTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        sm_.setGoalTemperature(100);
        sm_.setCycleLengthInMilliseconds(50);
        sm_.updateCurrentTemp(25);
        sm_.enableHeating(true);
    }
    // virtual void TearDown() {}
    HeatingSM sm_;
};

class HeatingSM_runRelayTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        sm_.setGoalTemperature(100);
        sm_.setCycleLengthInMilliseconds(50);
        sm_.updateCurrentTemp(25);
        sm_.enableHeating(true);
        sm_.updateCurrentTemp(25);
    }
    // virtual void TearDown() {}
    HeatingSM sm_;
};


// ************
// RUN TESTS
// ***********

// NOTE: these tests check each of the many possible state transitions.  In doing so, we  must sometimes
// traverse through other states.  Do not test these intermediate states, leave them for their own tests.

// CURRENT > GOAL > CYCLE
TEST_F(HeatingSM_hibernateTest, hibernate_initOrder_permutation_0)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    //update the temperature
    sm_.updateCurrentTemp(25);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
// CURRENT > CYCLE > GOAL
TEST_F(HeatingSM_hibernateTest, hibernate_initOrder_permutation_1)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    //update the temperature
    sm_.updateCurrentTemp(25);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
// GOAL > CURRENT > CYCLE
TEST_F(HeatingSM_hibernateTest, hibernate_initOrder_permutation_2)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    //update the temperature
    sm_.updateCurrentTemp(25);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
// GOAL > CYCLE > CURRENT
TEST_F(HeatingSM_hibernateTest, hibernate_initOrder_permutation_3)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    //update the temperature
    sm_.updateCurrentTemp(25);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
// CYCLE > GOAL > CURRENT
TEST_F(HeatingSM_hibernateTest, hibernate_initOrder_permutation_4)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    //update the temperature
    sm_.updateCurrentTemp(25);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
// CYCLE > CURRENT > GOAL
TEST_F(HeatingSM_hibernateTest, hibernate_initOrder_permutation_5)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    //update the temperature
    sm_.updateCurrentTemp(25);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // should still be in hibernate
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};


// ****************
// HIBERNATE STATES
// Possible:
// HIB > DET
// HIB > PUMPING
// ****************
TEST_F(HeatingSM_hibernateTest, hibernate_determineState)
{
    // we should initialize to not heating and in hibernation
    EXPECT_EQ(HeatingSM::HIBERNATE, sm_.getState());
    EXPECT_FALSE(sm_.isHeating());
    EXPECT_FALSE(sm_.runRelay());
    EXPECT_FALSE(sm_.runThyristor());
    //update the temperature
    sm_.updateCurrentTemp(25);
    // set a goal temperature
    sm_.setGoalTemperature(100);
    // set a cycle length
    sm_.setCycleLengthInMilliseconds(50);

    // move to DET state
    sm_.enableHeating(true);
    EXPECT_EQ(sm_.getState(), HeatingSM::DETERMINE_STATE);
    // check heat in DET state is off
    EXPECT_FALSE(sm_.runRelay());
    EXPECT_FALSE(sm_.runThyristor());
};

TEST_F(HeatingSM_hibernateTest, hibernate_pumping)
{
    // notify pump is running
    sm_.informOfPumping(true);
    // in HIB mode we never heat even when pumping
    EXPECT_FALSE(sm_.isHeating());
    EXPECT_FALSE(sm_.runRelay());
    EXPECT_FALSE(sm_.runThyristor());
};



// ****************
// DETERMINE_STATE STATES
// Possible:
// DET > HIBERNATE
// DET > RELAY
// DET > PULSE
// DET > COOLING
// DET > PUMPING
// ****************
TEST_F(HeatingSM_determineStateTest, determineState_hibernate)
{
    // move to HIBERNATE state
    sm_.enableHeating(false);
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
TEST_F(HeatingSM_determineStateTest, determineState_runRelay)
{
    // update temperature to relay range
    sm_.updateCurrentTemp(100-HeatingSM::RELAY_CUTOFF_OFFSET_CELSIUS-1);
    EXPECT_EQ(sm_.getState(), HeatingSM::HEATING_RELAY);
};
TEST_F(HeatingSM_determineStateTest, determineState_runPulse)
{
    // update temperature to relay range
    sm_.updateCurrentTemp(100 - HeatingSM::RELAY_CUTOFF_OFFSET_CELSIUS + 1);
    EXPECT_EQ(sm_.getState(), HeatingSM::HEATING_PULSE);
};
TEST_F(HeatingSM_determineStateTest, determineState_cooling)
{
    // update temperature to relay range
    sm_.updateCurrentTemp(100 + 1);
    EXPECT_EQ(sm_.getState(), HeatingSM::COOLING);
};
TEST_F(HeatingSM_determineStateTest, determineState_pumping)
{
    // update temperature to relay range
    sm_.informOfPumping(true);
    EXPECT_EQ(sm_.getState(), HeatingSM::PUMPING);
};

// ****************
// RUN_RELAY STATES
// Possible:
// RELAY > HIBERNATE
// RELAY > PULSE
// RELAY > COOLING
// RELAY > PUMPING
// ****************
TEST_F(HeatingSM_runRelayTest, runRelay_heating)
{
    // in relay state relay is running and thyristor is not
    EXPECT_TRUE(sm_.runRelay());
    EXPECT_FALSE(sm_.runThyristor());
};
TEST_F(HeatingSM_runRelayTest, runRelay_hibernate)
{
    sm_.enableHeating(false);
    EXPECT_EQ(sm_.getState(), HeatingSM::HIBERNATE);
};
TEST_F(HeatingSM_runRelayTest, runRelay_pulse)
{
    sm_.updateCurrentTemp(100-HeatingSM::RELAY_CUTOFF_OFFSET_CELSIUS+1);
    EXPECT_EQ(sm_.getState(), HeatingSM::HEATING_PULSE);
};
TEST_F(HeatingSM_runRelayTest, runRelay_cooling)
{
    sm_.updateCurrentTemp(100 + 1);
    EXPECT_EQ(sm_.getState(), HeatingSM::COOLING);
};
TEST_F(HeatingSM_runRelayTest, runRelay_pumping)
{
    sm_.informOfPumping(true);
    EXPECT_EQ(sm_.getState(), HeatingSM::PUMPING);
};