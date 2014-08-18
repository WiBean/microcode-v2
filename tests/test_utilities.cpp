#include "Utilities.h"


#include <gtest\gtest.h>


// test wibean::utils::boolToString
TEST(TestUtilities, boolToString)
{
	String trueStr = wibean::utils::boolToString(true);
	EXPECT_EQ(String("true"),trueStr);
	String falseStr = wibean::utils::boolToString(false);
	EXPECT_EQ(String("false"),falseStr);
}

//test wibean::utils::takeNext
TEST(TestUtilities, nextCharEmpty)
{
	String testVal = "";
	int outVal, retVal;
	retVal = wibean::utils::takeNext(testVal,0,outVal);
	EXPECT_EQ(-1, retVal);
}
TEST(TestUtilities, nextCharOnlyVal)
{
	String testVal = "34";
	int outVal, retVal;
	retVal = wibean::utils::takeNext(testVal,0,outVal);
	EXPECT_EQ(2, retVal);
	EXPECT_EQ(34, outVal);
}
TEST(TestUtilities, nextCharTrailingSeparator)
{
	String testVal = "35,";
	int outVal, retVal;
	retVal = wibean::utils::takeNext(testVal,0,outVal);
	EXPECT_EQ(2, retVal);
	EXPECT_EQ(35, outVal);
	retVal = wibean::utils::takeNext(testVal,retVal,outVal);
	EXPECT_EQ(-1, retVal);
}