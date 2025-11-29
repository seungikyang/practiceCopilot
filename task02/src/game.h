/**
 * @file game.h
 * @brief Rock-Paper-Scissors-Lizard-Spock game module
 *
 * This module implements the extended Rock-Paper-Scissors game with
 * Lizard and Spock options. Game rules:
 * - Rock crushes Scissors and crushes Lizard
 * - Paper covers Rock and disproves Spock
 * - Scissors cuts Paper and decapitates Lizard
 * - Lizard eats Paper and poisons Spock
 * - Spock vaporizes Rock and smashes Scissors
 *
 * @author Library Management System Team
 * @date 2025
 */

#ifndef GAME_H
#define GAME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Choice constants */
#define CHOICE_ROCK 0
#define CHOICE_SCISSORS 1
#define CHOICE_PAPER 2
#define CHOICE_LIZARD 3
#define CHOICE_SPOCK 4
#define CHOICE_COUNT 5
#define CHOICE_INVALID -1

/* Game result constants */
#define RESULT_TIE 0
#define RESULT_USER_WIN 1
#define RESULT_COMPUTER_WIN -1

/* Input validation constants */
#define MAX_INPUT_LENGTH 10
#define RESPONSE_YES 'y'
#define RESPONSE_NO 'n'

/**
 * @brief Get the name of a choice
 *
 * @param choice The choice constant (CHOICE_ROCK, etc.)
 * @return const char* The name of the choice, or "Invalid" if out of range
 *
 * @example
 * const char* name = get_choice_name(CHOICE_ROCK);  // Returns "Rock"
 */
const char* get_choice_name(int choice);

/**
 * @brief Generate a random computer choice
 *
 * This function generates a random choice between 0 and 4 (inclusive)
 * representing Rock, Scissors, Paper, Lizard, or Spock.
 *
 * @return int A random choice (0-4)
 *
 * @warning The random number generator must be initialized with srand()
 *          before calling this function
 *
 * @example
 * srand((unsigned int)time(NULL));
 * int choice = get_computer_choice();
 */
int get_computer_choice(void);

/**
 * @brief Determine the winner of the game
 *
 * Compares user and computer choices according to game rules and
 * determines the winner.
 *
 * @param user_choice User's choice (0-4)
 * @param computer_choice Computer's choice (0-4)
 * @return int RESULT_TIE (0) for tie, RESULT_USER_WIN (1) for user win,
 *         RESULT_COMPUTER_WIN (-1) for computer win
 *
 * @example
 * int result = determine_winner(CHOICE_ROCK, CHOICE_SCISSORS);
 * // Returns RESULT_USER_WIN (1)
 */
int determine_winner(int user_choice, int computer_choice);

/**
 * @brief Parse user input string to choice constant
 *
 * Converts a string input (e.g., "rock", "paper") to the corresponding
 * choice constant.
 *
 * @param input User input string
 * @return int Choice constant (0-4), or CHOICE_INVALID (-1) if invalid
 *
 * @warning The input string is expected to be lowercase
 *
 * @example
 * int choice = parse_user_input("rock");  // Returns CHOICE_ROCK (0)
 * int invalid = parse_user_input("xyz");  // Returns CHOICE_INVALID (-1)
 */
int parse_user_input(const char* input);

/**
 * @brief Check if user's choice beats computer's choice
 *
 * Internal helper function to determine if the user's choice
 * wins against the computer's choice according to game rules.
 *
 * @param user_choice User's choice (0-4)
 * @param computer_choice Computer's choice (0-4)
 * @return int 1 if user wins, 0 otherwise
 */
int user_wins_against(int user_choice, int computer_choice);

#ifdef __cplusplus
}
#endif

#endif /* GAME_H */
