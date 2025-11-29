/**
 * @file test_game_gtest.cpp
 * @brief Google Test unit tests for Rock-Paper-Scissors-Lizard-Spock game
 *
 * This file contains comprehensive unit tests for the game logic including:
 * - Choice name retrieval
 * - Winner determination for all combinations
 * - Input parsing and validation
 * - Computer choice generation
 * - Edge cases and error handling
 */

#include <gtest/gtest.h>

extern "C" {
#include "game.h"
}

/**
 * @class GameTest
 * @brief Test fixture for game functions
 */
class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup runs before each test
    }

    void TearDown() override {
        // Cleanup runs after each test
    }
};

/* ========================================================================
 * Choice Name Tests
 * ======================================================================== */

/**
 * @brief Test get_choice_name with valid choices
 *
 * Verifies that each choice constant returns the correct name string.
 */
TEST_F(GameTest, GetChoiceName_ValidChoices) {
    EXPECT_STREQ("Rock", get_choice_name(CHOICE_ROCK));
    EXPECT_STREQ("Scissors", get_choice_name(CHOICE_SCISSORS));
    EXPECT_STREQ("Paper", get_choice_name(CHOICE_PAPER));
    EXPECT_STREQ("Lizard", get_choice_name(CHOICE_LIZARD));
    EXPECT_STREQ("Spock", get_choice_name(CHOICE_SPOCK));
}

/**
 * @brief Test get_choice_name with invalid choices
 *
 * Verifies that out-of-range choices return "Invalid".
 */
TEST_F(GameTest, GetChoiceName_InvalidChoices) {
    EXPECT_STREQ("Invalid", get_choice_name(-1));
    EXPECT_STREQ("Invalid", get_choice_name(5));
    EXPECT_STREQ("Invalid", get_choice_name(100));
    EXPECT_STREQ("Invalid", get_choice_name(-999));
}

/* ========================================================================
 * Computer Choice Tests
 * ======================================================================== */

/**
 * @brief Test get_computer_choice returns valid range
 *
 * Generates multiple computer choices and verifies they are all
 * within the valid range [0, 4].
 */
TEST_F(GameTest, GetComputerChoice_ValidRange) {
    srand(12345);  // Fixed seed for reproducibility
    
    for (int i = 0; i < 100; i++) {
        int choice = get_computer_choice();
        EXPECT_GE(choice, 0);
        EXPECT_LT(choice, CHOICE_COUNT);
    }
}

/**
 * @brief Test get_computer_choice distribution
 *
 * Verifies that over many iterations, all choices are generated
 * (basic randomness check).
 */
TEST_F(GameTest, GetComputerChoice_AllChoicesGenerated) {
    srand(54321);  // Fixed seed
    
    bool found[CHOICE_COUNT] = {false};
    
    // Try up to 1000 times to generate all choices
    for (int i = 0; i < 1000; i++) {
        int choice = get_computer_choice();
        if (choice >= 0 && choice < CHOICE_COUNT) {
            found[choice] = true;
        }
    }
    
    // Verify all choices were generated at least once
    for (int i = 0; i < CHOICE_COUNT; i++) {
        EXPECT_TRUE(found[i]) << "Choice " << i << " was never generated";
    }
}

/* ========================================================================
 * Winner Determination Tests - Ties
 * ======================================================================== */

/**
 * @brief Test determine_winner for all tie scenarios
 *
 * Verifies that identical choices result in a tie.
 */
TEST_F(GameTest, DetermineWinner_AllTies) {
    EXPECT_EQ(RESULT_TIE, determine_winner(CHOICE_ROCK, CHOICE_ROCK));
    EXPECT_EQ(RESULT_TIE, determine_winner(CHOICE_SCISSORS, CHOICE_SCISSORS));
    EXPECT_EQ(RESULT_TIE, determine_winner(CHOICE_PAPER, CHOICE_PAPER));
    EXPECT_EQ(RESULT_TIE, determine_winner(CHOICE_LIZARD, CHOICE_LIZARD));
    EXPECT_EQ(RESULT_TIE, determine_winner(CHOICE_SPOCK, CHOICE_SPOCK));
}

/* ========================================================================
 * Winner Determination Tests - Rock Wins
 * ======================================================================== */

/**
 * @brief Test Rock wins scenarios
 *
 * Rock crushes Scissors and crushes Lizard.
 */
TEST_F(GameTest, DetermineWinner_RockWins) {
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_ROCK, CHOICE_SCISSORS));
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_ROCK, CHOICE_LIZARD));
}

/**
 * @brief Test Rock loses scenarios
 *
 * Rock loses to Paper and Spock.
 */
TEST_F(GameTest, DetermineWinner_RockLoses) {
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_ROCK, CHOICE_PAPER));
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_ROCK, CHOICE_SPOCK));
}

/* ========================================================================
 * Winner Determination Tests - Paper Wins
 * ======================================================================== */

/**
 * @brief Test Paper wins scenarios
 *
 * Paper covers Rock and disproves Spock.
 */
TEST_F(GameTest, DetermineWinner_PaperWins) {
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_PAPER, CHOICE_ROCK));
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_PAPER, CHOICE_SPOCK));
}

/**
 * @brief Test Paper loses scenarios
 *
 * Paper loses to Scissors and Lizard.
 */
TEST_F(GameTest, DetermineWinner_PaperLoses) {
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_PAPER, CHOICE_SCISSORS));
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_PAPER, CHOICE_LIZARD));
}

/* ========================================================================
 * Winner Determination Tests - Scissors Wins
 * ======================================================================== */

/**
 * @brief Test Scissors wins scenarios
 *
 * Scissors cuts Paper and decapitates Lizard.
 */
TEST_F(GameTest, DetermineWinner_ScissorsWins) {
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_SCISSORS, CHOICE_PAPER));
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_SCISSORS, CHOICE_LIZARD));
}

/**
 * @brief Test Scissors loses scenarios
 *
 * Scissors loses to Rock and Spock.
 */
TEST_F(GameTest, DetermineWinner_ScissorsLoses) {
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_SCISSORS, CHOICE_ROCK));
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_SCISSORS, CHOICE_SPOCK));
}

/* ========================================================================
 * Winner Determination Tests - Lizard Wins
 * ======================================================================== */

/**
 * @brief Test Lizard wins scenarios
 *
 * Lizard eats Paper and poisons Spock.
 */
TEST_F(GameTest, DetermineWinner_LizardWins) {
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_LIZARD, CHOICE_PAPER));
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_LIZARD, CHOICE_SPOCK));
}

/**
 * @brief Test Lizard loses scenarios
 *
 * Lizard loses to Rock and Scissors.
 */
TEST_F(GameTest, DetermineWinner_LizardLoses) {
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_LIZARD, CHOICE_ROCK));
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_LIZARD, CHOICE_SCISSORS));
}

/* ========================================================================
 * Winner Determination Tests - Spock Wins
 * ======================================================================== */

/**
 * @brief Test Spock wins scenarios
 *
 * Spock vaporizes Rock and smashes Scissors.
 */
TEST_F(GameTest, DetermineWinner_SpockWins) {
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_SPOCK, CHOICE_ROCK));
    EXPECT_EQ(RESULT_USER_WIN, determine_winner(CHOICE_SPOCK, CHOICE_SCISSORS));
}

/**
 * @brief Test Spock loses scenarios
 *
 * Spock loses to Paper and Lizard.
 */
TEST_F(GameTest, DetermineWinner_SpockLoses) {
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_SPOCK, CHOICE_PAPER));
    EXPECT_EQ(RESULT_COMPUTER_WIN, determine_winner(CHOICE_SPOCK, CHOICE_LIZARD));
}

/* ========================================================================
 * Input Parsing Tests
 * ======================================================================== */

/**
 * @brief Test parse_user_input with valid lowercase inputs
 *
 * Verifies that all valid input strings are correctly parsed.
 */
TEST_F(GameTest, ParseUserInput_ValidInputs) {
    EXPECT_EQ(CHOICE_ROCK, parse_user_input("rock"));
    EXPECT_EQ(CHOICE_SCISSORS, parse_user_input("scissors"));
    EXPECT_EQ(CHOICE_PAPER, parse_user_input("paper"));
    EXPECT_EQ(CHOICE_LIZARD, parse_user_input("lizard"));
    EXPECT_EQ(CHOICE_SPOCK, parse_user_input("spock"));
}

/**
 * @brief Test parse_user_input with invalid inputs
 *
 * Verifies that invalid strings return CHOICE_INVALID.
 */
TEST_F(GameTest, ParseUserInput_InvalidInputs) {
    EXPECT_EQ(CHOICE_INVALID, parse_user_input("invalid"));
    EXPECT_EQ(CHOICE_INVALID, parse_user_input(""));
    EXPECT_EQ(CHOICE_INVALID, parse_user_input("roc"));   // Incomplete
    EXPECT_EQ(CHOICE_INVALID, parse_user_input("rocks")); // Extra character
}

/**
 * @brief Test parse_user_input accepts uppercase/mixed-case and trimmed spaces
 */
TEST_F(GameTest, ParseUserInput_CasingAndWhitespace) {
    EXPECT_EQ(CHOICE_ROCK, parse_user_input("ROCK"));      // Uppercase
    EXPECT_EQ(CHOICE_ROCK, parse_user_input("Rock"));      // Mixed case
    EXPECT_EQ(CHOICE_ROCK, parse_user_input(" rock"));     // Leading space
    EXPECT_EQ(CHOICE_ROCK, parse_user_input("rock "));     // Trailing space
    EXPECT_EQ(CHOICE_SPOCK, parse_user_input("  SpOcK  ")); // Mixed + spaces
}

/**
 * @brief Test parse_user_input numeric inputs 0-4 and 1-5
 */
TEST_F(GameTest, ParseUserInput_NumericInputs) {
    EXPECT_EQ(CHOICE_ROCK, parse_user_input("0"));
    EXPECT_EQ(CHOICE_SCISSORS, parse_user_input("1"));
    EXPECT_EQ(CHOICE_PAPER, parse_user_input("2"));
    EXPECT_EQ(CHOICE_LIZARD, parse_user_input("3"));
    EXPECT_EQ(CHOICE_SPOCK, parse_user_input("4"));
    // 1-5 mapping (only affects input '5')
    EXPECT_EQ(CHOICE_SPOCK, parse_user_input("5"));
}

/**
 * @brief Test parse_user_input Korean aliases
 */
TEST_F(GameTest, ParseUserInput_KoreanAliases) {
    EXPECT_EQ(CHOICE_ROCK, parse_user_input("바위"));
    EXPECT_EQ(CHOICE_SCISSORS, parse_user_input("가위"));
    EXPECT_EQ(CHOICE_PAPER, parse_user_input("보"));
    EXPECT_EQ(CHOICE_LIZARD, parse_user_input("도마뱀"));
    EXPECT_EQ(CHOICE_SPOCK, parse_user_input("스팍"));
    EXPECT_EQ(CHOICE_SPOCK, parse_user_input("스포크"));
}

/**
 * @brief Test parse_user_input with NULL input
 *
 * Verifies that NULL input is handled safely.
 */
TEST_F(GameTest, ParseUserInput_NullInput) {
    EXPECT_EQ(CHOICE_INVALID, parse_user_input(NULL));
}

/* ========================================================================
 * Helper Function Tests
 * ======================================================================== */

/**
 * @brief Test user_wins_against for all winning combinations
 *
 * Verifies the internal helper function correctly identifies
 * all winning scenarios.
 */
TEST_F(GameTest, UserWinsAgainst_AllWinningCombinations) {
    // Rock wins
    EXPECT_EQ(1, user_wins_against(CHOICE_ROCK, CHOICE_SCISSORS));
    EXPECT_EQ(1, user_wins_against(CHOICE_ROCK, CHOICE_LIZARD));
    
    // Paper wins
    EXPECT_EQ(1, user_wins_against(CHOICE_PAPER, CHOICE_ROCK));
    EXPECT_EQ(1, user_wins_against(CHOICE_PAPER, CHOICE_SPOCK));
    
    // Scissors wins
    EXPECT_EQ(1, user_wins_against(CHOICE_SCISSORS, CHOICE_PAPER));
    EXPECT_EQ(1, user_wins_against(CHOICE_SCISSORS, CHOICE_LIZARD));
    
    // Lizard wins
    EXPECT_EQ(1, user_wins_against(CHOICE_LIZARD, CHOICE_PAPER));
    EXPECT_EQ(1, user_wins_against(CHOICE_LIZARD, CHOICE_SPOCK));
    
    // Spock wins
    EXPECT_EQ(1, user_wins_against(CHOICE_SPOCK, CHOICE_ROCK));
    EXPECT_EQ(1, user_wins_against(CHOICE_SPOCK, CHOICE_SCISSORS));
}

/**
 * @brief Test user_wins_against for all losing combinations
 *
 * Verifies the internal helper function correctly identifies
 * all losing scenarios.
 */
TEST_F(GameTest, UserWinsAgainst_AllLosingCombinations) {
    // Rock loses
    EXPECT_EQ(0, user_wins_against(CHOICE_ROCK, CHOICE_PAPER));
    EXPECT_EQ(0, user_wins_against(CHOICE_ROCK, CHOICE_SPOCK));
    
    // Paper loses
    EXPECT_EQ(0, user_wins_against(CHOICE_PAPER, CHOICE_SCISSORS));
    EXPECT_EQ(0, user_wins_against(CHOICE_PAPER, CHOICE_LIZARD));
    
    // Scissors loses
    EXPECT_EQ(0, user_wins_against(CHOICE_SCISSORS, CHOICE_ROCK));
    EXPECT_EQ(0, user_wins_against(CHOICE_SCISSORS, CHOICE_SPOCK));
    
    // Lizard loses
    EXPECT_EQ(0, user_wins_against(CHOICE_LIZARD, CHOICE_ROCK));
    EXPECT_EQ(0, user_wins_against(CHOICE_LIZARD, CHOICE_SCISSORS));
    
    // Spock loses
    EXPECT_EQ(0, user_wins_against(CHOICE_SPOCK, CHOICE_PAPER));
    EXPECT_EQ(0, user_wins_against(CHOICE_SPOCK, CHOICE_LIZARD));
}

/**
 * @brief Test user_wins_against for tie scenarios
 *
 * Verifies that identical choices return 0 (not a win).
 */
TEST_F(GameTest, UserWinsAgainst_TieScenarios) {
    EXPECT_EQ(0, user_wins_against(CHOICE_ROCK, CHOICE_ROCK));
    EXPECT_EQ(0, user_wins_against(CHOICE_SCISSORS, CHOICE_SCISSORS));
    EXPECT_EQ(0, user_wins_against(CHOICE_PAPER, CHOICE_PAPER));
    EXPECT_EQ(0, user_wins_against(CHOICE_LIZARD, CHOICE_LIZARD));
    EXPECT_EQ(0, user_wins_against(CHOICE_SPOCK, CHOICE_SPOCK));
}

/* ========================================================================
 * Comprehensive Matrix Tests
 * ======================================================================== */

/**
 * @brief Test complete game matrix
 *
 * Verifies all 25 possible game combinations produce correct results.
 * This is a comprehensive test that covers the entire game logic.
 */
TEST_F(GameTest, DetermineWinner_CompleteMatrix) {
    // Expected results matrix: [user_choice][computer_choice]
    // 0 = tie, 1 = user wins, -1 = computer wins
    const int expected[5][5] = {
        // vs: Rock  Scissors  Paper  Lizard  Spock
        {    0,      1,       -1,     1,     -1  },  // Rock
        {   -1,      0,        1,     1,     -1  },  // Scissors
        {    1,     -1,        0,    -1,      1  },  // Paper
        {   -1,     -1,        1,     0,      1  },  // Lizard
        {    1,      1,       -1,    -1,      0  }   // Spock
    };
    
    for (int user = 0; user < CHOICE_COUNT; user++) {
        for (int computer = 0; computer < CHOICE_COUNT; computer++) {
            int result = determine_winner(user, computer);
            EXPECT_EQ(expected[user][computer], result)
                << "Failed for user=" << get_choice_name(user)
                << " vs computer=" << get_choice_name(computer);
        }
    }
}

/**
 * @brief Test game symmetry
 *
 * Verifies that swapping user and computer choices produces
 * the opposite result (except for ties).
 */
TEST_F(GameTest, DetermineWinner_Symmetry) {
    for (int user = 0; user < CHOICE_COUNT; user++) {
        for (int computer = 0; computer < CHOICE_COUNT; computer++) {
            if (user == computer) {
                continue;  // Skip ties
            }
            
            int result1 = determine_winner(user, computer);
            int result2 = determine_winner(computer, user);
            
            EXPECT_EQ(-result1, result2)
                << "Symmetry failed for " << get_choice_name(user)
                << " vs " << get_choice_name(computer);
        }
    }
}

/* ========================================================================
 * Main Test Runner
 * ======================================================================== */

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
