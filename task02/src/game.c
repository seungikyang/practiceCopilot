/**
 * @file game.c
 * @brief Implementation of Rock-Paper-Scissors-Lizard-Spock game logic
 */

#include "game.h"
#include <stdlib.h>
#include <string.h>

/* Choice name lookup table */
static const char* CHOICE_NAMES[CHOICE_COUNT] = {
    "Rock",
    "Scissors",
    "Paper",
    "Lizard",
    "Spock"
};

/* Invalid choice name */
static const char* INVALID_CHOICE_NAME = "Invalid";

/**
 * @brief Get the name of a choice
 */
const char* get_choice_name(int choice) {
    if (choice >= 0 && choice < CHOICE_COUNT) {
        return CHOICE_NAMES[choice];
    }
    return INVALID_CHOICE_NAME;
}

/**
 * @brief Generate a random computer choice
 */
int get_computer_choice(void) {
    return rand() % CHOICE_COUNT;
}

/**
 * @brief Check if user's choice beats computer's choice
 */
int user_wins_against(int user_choice, int computer_choice) {
    /* Rock crushes Scissors and Lizard */
    if (user_choice == CHOICE_ROCK && 
        (computer_choice == CHOICE_SCISSORS || computer_choice == CHOICE_LIZARD)) {
        return 1;
    }
    
    /* Paper covers Rock and disproves Spock */
    if (user_choice == CHOICE_PAPER && 
        (computer_choice == CHOICE_ROCK || computer_choice == CHOICE_SPOCK)) {
        return 1;
    }
    
    /* Scissors cuts Paper and decapitates Lizard */
    if (user_choice == CHOICE_SCISSORS && 
        (computer_choice == CHOICE_PAPER || computer_choice == CHOICE_LIZARD)) {
        return 1;
    }
    
    /* Lizard eats Paper and poisons Spock */
    if (user_choice == CHOICE_LIZARD && 
        (computer_choice == CHOICE_PAPER || computer_choice == CHOICE_SPOCK)) {
        return 1;
    }
    
    /* Spock vaporizes Rock and smashes Scissors */
    if (user_choice == CHOICE_SPOCK && 
        (computer_choice == CHOICE_ROCK || computer_choice == CHOICE_SCISSORS)) {
        return 1;
    }
    
    return 0;
}

/**
 * @brief Determine the winner of the game
 */
int determine_winner(int user_choice, int computer_choice) {
    if (user_choice == computer_choice) {
        return RESULT_TIE;
    }
    
    if (user_wins_against(user_choice, computer_choice)) {
        return RESULT_USER_WIN;
    }
    
    return RESULT_COMPUTER_WIN;
}

/**
 * @brief Parse user input string to choice constant
 */
int parse_user_input(const char* input) {
    if (input == NULL) {
        return CHOICE_INVALID;
    }
    
    if (strcmp(input, "rock") == 0) {
        return CHOICE_ROCK;
    }
    if (strcmp(input, "scissors") == 0) {
        return CHOICE_SCISSORS;
    }
    if (strcmp(input, "paper") == 0) {
        return CHOICE_PAPER;
    }
    if (strcmp(input, "lizard") == 0) {
        return CHOICE_LIZARD;
    }
    if (strcmp(input, "spock") == 0) {
        return CHOICE_SPOCK;
    }
    
    return CHOICE_INVALID;
}
