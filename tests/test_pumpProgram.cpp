#include "UniformlySpacedOutputSortedLookupTable.h"

#include <gtest\gtest.h>
#include "PumpProgram.h"

// ************
// INIT ODD TEST FIXTURE 
// ***********
class PumpProgramTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        // transfer our values from onFor/offFor, into onTimes and offTimes;
        PumpProgram<5>::PUMP_TIME_TYPE onTimes[PumpProgram<5>::PUMP_STEPS];
        PumpProgram<5>::PUMP_TIME_TYPE offTimes[PumpProgram<5>::PUMP_STEPS];
        PumpProgram<5>::PUMP_TIME_TYPE onForMillis[PumpProgram<5>::PUMP_STEPS] = {1000,2000,3000,0,0};
        PumpProgram<5>::PUMP_TIME_TYPE offForMillis[PumpProgram<5>::PUMP_STEPS] = {2000,3000,4000,0,0};

        offset_ = 1000; // 1 second offset
        onTimes[0] = offset_;
        offTimes[0] = onTimes[0] + onForMillis[0];
        for (uint32_t k = 1; k<pump_.PUMP_STEPS; ++k) {
            onTimes[k] = offTimes[k - 1] + offForMillis[k - 1];
            offTimes[k] = onTimes[k] + onForMillis[k];
        }
        pump_.setProgram(5, onTimes, offTimes);
    };
    // virtual void TearDown() {}
    PumpProgram<5> pump_;
    uint32_t offset_;
};

//
//void setProgram(uint16_t numel, uint32_t onTimes[], uint32_t offTimes[]) {

 
// ************
// RUN TESTS
// ***********
TEST_F(PumpProgramTest, isPumpingAt)
{
    // on from [offset+0,offset+1000)
    EXPECT_FALSE(pump_.isPumpingAt(0));
    EXPECT_TRUE(pump_.isPumpingAt(offset_+0));
    EXPECT_TRUE(pump_.isPumpingAt(offset_ + 1));
    // off at exactly 1 second
    EXPECT_FALSE(pump_.isPumpingAt(offset_ + 1000));
    EXPECT_FALSE(pump_.isPumpingAt(offset_ + 1100));
    // back on at 3
    EXPECT_TRUE(pump_.isPumpingAt(offset_ + 3001));
    // off at 5
    EXPECT_FALSE(pump_.isPumpingAt(offset_ + 5001));
    // on at 8
    EXPECT_TRUE(pump_.isPumpingAt(offset_ + 8001));
    // off at 11
    EXPECT_FALSE(pump_.isPumpingAt(offset_ + 11001));
    // off way after that
    EXPECT_FALSE(pump_.isPumpingAt(offset_ + 65536));
}

TEST_F(PumpProgramTest, isValveOpenAt)
{
    // on from [offset+0,offset+END)
    EXPECT_FALSE(pump_.isValveOpenAt(0));
    EXPECT_TRUE(pump_.isValveOpenAt(offset_));
    EXPECT_TRUE(pump_.isValveOpenAt(offset_ + 1));
    EXPECT_TRUE(pump_.isValveOpenAt(offset_ + 2000));
    EXPECT_FALSE(pump_.isValveOpenAt(65536));
}

//void validateProgram(uint16_t numel, uint32_t onTimes[], uint32_t offTimes[]) {
TEST_F(PumpProgramTest, validateProgram)
{
    // on from [offset+0,offset+END)
    EXPECT_FALSE(pump_.isValveOpenAt(0));
    EXPECT_TRUE(pump_.isValveOpenAt(offset_));
    EXPECT_TRUE(pump_.isValveOpenAt(offset_ + 1));
    EXPECT_TRUE(pump_.isValveOpenAt(offset_ + 2000));
    EXPECT_FALSE(pump_.isValveOpenAt(65536));
}