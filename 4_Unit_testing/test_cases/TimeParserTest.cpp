
//This tests the parser implementation

#include <gtest/gtest.h>
#include "../TimeParser.h"

//Tests for:
// Correct time HHMMSS = 246060
// Incorrect lenght
// Correct lenght = 6 characters
// Only zeroes
// Null
// Only digits


// Test suite: TimeParserTest
TEST(TimeParserTest, TestCaseCorrectTime) {

    // Test with correct time string
    char time_test[] = "141205";
    EXPECT_EQ(time_parse(time_test) , 725 );

    char time_test2[] = "000105";
    EXPECT_EQ(time_parse(time_test2) , 65 );
}

TEST(TimeParserTest, TestCaseIncorrectTime) {

    //Test with incorrect timecd b  
    char time_test[] = "000077";
    EXPECT_EQ(time_parse(time_test) , TIME_VALUE_ERROR );

    char time_test2[] = "008800";
    EXPECT_EQ(time_parse(time_test2) , TIME_VALUE_ERROR );

}

TEST(TimeParserTest, TestCaseIncorrectLenght) {
    char time_test[] = "12345";
    EXPECT_EQ(time_parse(time_test) , TIME_LEN_ERROR );

    char time_test2[] = "1234567";
    EXPECT_EQ(time_parse(time_test2) , TIME_LEN_ERROR );

}

TEST(TimeParserTest, TestCaseZeroes) {
    char time_test[] = "000000";
    EXPECT_EQ(time_parse(time_test) , TIME_VALUE_ERROR);
}


TEST(TimeParserTest, TestNull) {
    EXPECT_EQ(time_parse(NULL) , TIME_NULL_ERROR);
}

TEST(TimeParserTest, TestDigit) {
    char time_test[] = "abcdef";
    EXPECT_EQ(time_parse(time_test) , TIME_DIGIT_ERROR);

    char time_test2[] = "123a56";
    EXPECT_EQ(time_parse(time_test2) , TIME_DIGIT_ERROR);
}

// https://google.github.io/googletest/reference/testing.html
// https://google.github.io/googletest/reference/assertions.html
