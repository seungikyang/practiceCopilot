/**
 * @file main.c
 * @brief Main program for Rock-Paper-Scissors-Lizard-Spock game
 *
 * This program implements an interactive game where the user plays
 * Rock-Paper-Scissors-Lizard-Spock against the computer.
 */

#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/* Error message constants */
static const char* MSG_INVALID_CHOICE = "Invalid choice. Please try again.\n";
static const char* MSG_INVALID_RESPONSE = "Invalid input. Please enter 'y' or 'n'.\n";
static const char* MSG_YOU_WIN = "You win!\n";
static const char* MSG_YOU_LOSE = "You lose!\n";
static const char* MSG_TIE = "It's a tie!\n";

/* Prompt message constants */
static const char* PROMPT_CHOICE = "Enter your choice (rock, scissors, paper, lizard, spock): ";
static const char* PROMPT_PLAY_AGAIN = "Do you want to play again? (y/n): ";
static const char* MSG_COMPUTER_CHOSE = "Computer chose: %s\n";

/**
 * @brief Display the game result message
 *
 * @param result The game result (RESULT_TIE, RESULT_USER_WIN, or RESULT_COMPUTER_WIN)
 */
static void display_result(int result) {
    if (result == RESULT_USER_WIN) {
        printf("%s", MSG_YOU_WIN);
    } else if (result == RESULT_COMPUTER_WIN) {
        printf("%s", MSG_YOU_LOSE);
    } else {
        printf("%s", MSG_TIE);
    }
}

/**
 * @brief Get validated user input
 *
 * Prompts the user for input and validates it.
 *
 * @param buffer Buffer to store the input
 * @param buffer_size Size of the buffer
 * @return int 0 on success, -1 on input error
 */
static int get_user_input(char* buffer, size_t buffer_size) {
    if (buffer == NULL || buffer_size == 0) {
        return -1;
    }
    
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return -1;
    }
    
    /* Remove newline character if present */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return 0;
}

/**
 * @brief Play a single round of the game
 *
 * @return int 0 to continue playing, 1 to quit
 */
static int play_round(void) {
    char user_input[MAX_INPUT_LENGTH];
    
    /* Get user's choice */
    printf("%s", PROMPT_CHOICE);
    if (get_user_input(user_input, sizeof(user_input)) != 0) {
        printf("%s", MSG_INVALID_CHOICE);
        return 0;
    }
    
    int user_choice = parse_user_input(user_input);
    if (user_choice == CHOICE_INVALID) {
        printf("%s", MSG_INVALID_CHOICE);
        return 0;
    }
    
    /* Get computer's choice */
    int computer_choice = get_computer_choice();
    printf(MSG_COMPUTER_CHOSE, get_choice_name(computer_choice));
    
    /* Determine and display result */
    int result = determine_winner(user_choice, computer_choice);
    display_result(result);
    
    /* Ask if user wants to play again */
    while (1) {
        printf("%s", PROMPT_PLAY_AGAIN);
        if (get_user_input(user_input, sizeof(user_input)) != 0) {
            printf("%s", MSG_INVALID_RESPONSE);
            continue;
        }
        
        if (strlen(user_input) == 1) {
            if (user_input[0] == RESPONSE_YES) {
                return 0;  /* Continue playing */
            }
            if (user_input[0] == RESPONSE_NO) {
                return 1;  /* Quit */
            }
        }
        
        printf("%s", MSG_INVALID_RESPONSE);
    }
}

/**
 * @brief Main entry point
 */
int main(void) {
    /* Initialize random number generator */
    srand((unsigned int)time(NULL));
    
    /* Game loop */
    while (1) {
        int quit = play_round();
        if (quit) {
            break;
        }
    }
    
    return EXIT_SUCCESS;
}
