#include "UniformlySpacedOutputSortedLookupTable.h"

#include <gtest\gtest.h>

// ************
// INIT DUMMY ODD CLASS 
// ***********
class DummySortedLookupTableOddLength : public UniformlySpacedOutputSortedLookupTable<uint16_t, uint16_t, float, float>
{
public:
	DummySortedLookupTableOddLength() {
		OUTPUT_MIN = 0;
		OUTPUT_TABLE_DELTA = 0.5;
		OUTPUT_TABLE_DELTA_INTERP_PREC = 0.5;
	};
	float getTemperature(uint16_t reading);
    // LUT for thermistor
private:
	static uint16_t const INPUT_SIZE = 5;
	static uint16_t const INPUTS[INPUT_SIZE]; // non-integral init later
protected:
	virtual uint16_t getArrSize() { return INPUT_SIZE; };
	virtual uint16_t const* getArr() { return INPUTS; };
};
uint16_t const DummySortedLookupTableOddLength::INPUTS[INPUT_SIZE] = { 1, 3, 5, 7, 9 };
float DummySortedLookupTableOddLength::getTemperature(uint16_t const reading) {
  auto const lowHighIndex = binarySearchBounds(reading);
  //auto const lowHighIndex = linearSearchBounds(reading);
  uint16_t const lowValue = INPUTS[lowHighIndex.first];
  // check for special cases
  if (lowHighIndex.first == lowHighIndex.second) {
	  return computeOutputValue(lowHighIndex.first, 0);
  }
  else {
	  uint16_t const highValue = INPUTS[lowHighIndex.second];
	  return computeOutputValue(lowHighIndex.first, computeInterpFactor(lowValue, highValue, reading));
  }
  
}
// ************
// END DUMMY ODD CLASS 
// ***********

// ************
// INIT DUMMY EVEN CLASS 
// ***********
class DummySortedLookupTableEvenLength : public UniformlySpacedOutputSortedLookupTable<uint16_t, uint16_t, float, float>
{
public:
	DummySortedLookupTableEvenLength() {
		OUTPUT_MIN = 0;
		OUTPUT_TABLE_DELTA = 0.5;
		OUTPUT_TABLE_DELTA_INTERP_PREC = 0.5;
	};
	float getTemperature(uint16_t reading);
	// LUT for thermistor
private:
	static uint16_t const INPUT_SIZE = 6;
	static uint16_t const INPUTS[INPUT_SIZE]; // non-integral init later
protected:
	virtual uint16_t getArrSize() { return INPUT_SIZE; };
	virtual uint16_t const* getArr() { return INPUTS; };
};
uint16_t const DummySortedLookupTableEvenLength::INPUTS[INPUT_SIZE] = { 1, 3, 5, 7, 9, 11 };
float DummySortedLookupTableEvenLength::getTemperature(uint16_t const reading) {
	auto const lowHighIndex = binarySearchBounds(reading);
	//auto const lowHighIndex = linearSearchBounds(reading);
	uint16_t const lowValue = INPUTS[lowHighIndex.first];
	// check for special cases
	if (lowHighIndex.first == lowHighIndex.second) {
		return computeOutputValue(lowHighIndex.first, 0);
	}
	else {
		uint16_t const highValue = INPUTS[lowHighIndex.second];
		return computeOutputValue(lowHighIndex.first, computeInterpFactor(lowValue, highValue, reading));
	}
}
// ************
// END DUMMY EVEN CLASS 
// ***********

// ************
// INIT ODD TEST FIXTURE 
// ***********
class SortedLookupTableTestOddLength : public ::testing::Test {
protected:
	virtual void SetUp() {
	}
	// virtual void TearDown() {}
	DummySortedLookupTableOddLength lut_;
};

// ************
// INIT EVEN TEST FIXTURE 
// ***********
class SortedLookupTableTestEvenLength : public ::testing::Test {
protected:
	virtual void SetUp() {
	}
	// virtual void TearDown() {}
	DummySortedLookupTableEvenLength lut_;
};


// ************
// RUN ODD TESTS 
// ***********
TEST_F(SortedLookupTableTestOddLength, outOfBoundsLow)
{
	auto tVal = lut_.getTemperature(0);
	EXPECT_EQ(tVal, 0);
}
TEST_F(SortedLookupTableTestOddLength, onLowerBound)
{
	auto tVal = lut_.getTemperature(1);
	EXPECT_EQ(tVal, 0.0f);
}
TEST_F(SortedLookupTableTestOddLength, justInsideLowerBound)
{
	auto tVal = lut_.getTemperature(2);
	EXPECT_EQ(tVal, 0.25f);
}
TEST_F(SortedLookupTableTestOddLength, oneInsideLowerBound)
{
	auto tVal = lut_.getTemperature(3);
	EXPECT_EQ(tVal, 0.5f);
}
TEST_F(SortedLookupTableTestOddLength, justBelowHalf)
{
	auto tVal = lut_.getTemperature(4);
	EXPECT_EQ(tVal, 0.75f);
}
TEST_F(SortedLookupTableTestOddLength, onHalf)
{
	auto tVal = lut_.getTemperature(5);
	EXPECT_EQ(tVal, 1.0f);
}
TEST_F(SortedLookupTableTestOddLength, justAboveHalf)
{
	auto tVal = lut_.getTemperature(6);
	EXPECT_EQ(tVal, 1.25f);
}
TEST_F(SortedLookupTableTestOddLength, oneInsideUpperBound)
{
	auto tVal = lut_.getTemperature(7);
	EXPECT_EQ(tVal, 1.5f);
}
TEST_F(SortedLookupTableTestOddLength, justInsideUpperBound)
{
	auto tVal = lut_.getTemperature(8);
	EXPECT_EQ(tVal, 1.75f);
}
TEST_F(SortedLookupTableTestOddLength, onUpperBound)
{
	auto tVal = lut_.getTemperature(9);
	EXPECT_EQ(tVal, 2.0f);
}
TEST_F(SortedLookupTableTestOddLength, outOfBoundsHigh)
{
	auto tVal = lut_.getTemperature(65535);
	EXPECT_EQ(tVal, 2.0);
	// our LUT uses an unsigned value type, check for correct behavior on roll-over
	tVal = lut_.getTemperature(-1);
	EXPECT_EQ(tVal, 2.0);
}


// ************
// RUN EVEN TESTS 
// ***********
TEST_F(SortedLookupTableTestEvenLength, outOfBoundsLow)
{
	auto tVal = lut_.getTemperature(0);
	EXPECT_EQ(tVal, 0);
}
TEST_F(SortedLookupTableTestEvenLength, onLowerBound)
{
	auto tVal = lut_.getTemperature(1);
	EXPECT_EQ(tVal, 0.0f);
}
TEST_F(SortedLookupTableTestEvenLength, justInsideLowerBound)
{
	auto tVal = lut_.getTemperature(2);
	EXPECT_EQ(tVal, 0.25f);
}
TEST_F(SortedLookupTableTestEvenLength, oneInsideLowerBound)
{
	auto tVal = lut_.getTemperature(3);
	EXPECT_EQ(tVal, 0.5f);
}
TEST_F(SortedLookupTableTestEvenLength, justBelowIntHalf)
{
	auto tVal = lut_.getTemperature(4);
	EXPECT_EQ(tVal, 0.75f);
}
TEST_F(SortedLookupTableTestEvenLength, onIntHalf)
{
	auto tVal = lut_.getTemperature(5);
	EXPECT_EQ(tVal, 1.0f);
}
TEST_F(SortedLookupTableTestEvenLength, justAboveIntHalf)
{
	auto tVal = lut_.getTemperature(6);
	EXPECT_EQ(tVal, 1.25f);
}
TEST_F(SortedLookupTableTestEvenLength, oneInsideUpperBound)
{
	auto tVal = lut_.getTemperature(9);
	EXPECT_EQ(tVal, 2.0f);
}
TEST_F(SortedLookupTableTestEvenLength, justInsideUpperBound)
{
	auto tVal = lut_.getTemperature(10);
	EXPECT_EQ(tVal, 2.25f);
}
TEST_F(SortedLookupTableTestEvenLength, onUpperBound)
{
	auto tVal = lut_.getTemperature(11);
	EXPECT_EQ(tVal, 2.5f);
}
TEST_F(SortedLookupTableTestEvenLength, outOfBoundsHigh)
{
	auto tVal = lut_.getTemperature(65535);
	EXPECT_EQ(tVal, 2.5);
	// our LUT uses an unsigned value type, check for correct behavior on roll-over
	tVal = lut_.getTemperature(-1);
	EXPECT_EQ(tVal, 2.5);
}