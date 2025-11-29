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

    /* Normalize: trim spaces and convert to lowercase copy */
    char buf[32];
    size_t len = strlen(input);
    /* Trim leading/trailing spaces */
    size_t start = 0;
    while (start < len && (input[start] == ' ' || input[start] == '\t')) start++;
    size_t end = len;
    while (end > start && (input[end - 1] == ' ' || input[end - 1] == '\t')) end--;
    size_t n = end > start ? (end - start) : 0;
    if (n >= sizeof(buf)) n = sizeof(buf) - 1;
    for (size_t i = 0; i < n; ++i) {
        char c = input[start + i];
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
        buf[i] = c;
    }
    buf[n] = '\0';
    
    /* Support numeric input: 0-4 or 1-5 mapping to choices */
    if (buf[0] >= '0' && buf[0] <= '9' && buf[1] == '\0') {
        int nval = buf[0] - '0';
        if (nval >= 0 && nval <= 4) {
            return nval; /* 0-4 maps directly to CHOICE_* */
        }
        if (nval >= 1 && nval <= 5) {
            return nval - 1; /* 1-5 maps to 0-4 */
        }
    }

    /* English aliases */
    if (strcmp(buf, "rock") == 0) {
        return CHOICE_ROCK;
    }
    if (strcmp(buf, "scissors") == 0) {
        return CHOICE_SCISSORS;
    }
    if (strcmp(buf, "paper") == 0) {
        return CHOICE_PAPER;
    }
    if (strcmp(buf, "lizard") == 0) {
        return CHOICE_LIZARD;
    }
    if (strcmp(buf, "spock") == 0) {
        return CHOICE_SPOCK;
    }

    /* Korean aliases */
    if (strcmp(buf, "바위") == 0) {
        return CHOICE_ROCK;
    }
    if (strcmp(buf, "가위") == 0) {
        return CHOICE_SCISSORS;
    }
    if (strcmp(buf, "보") == 0) {
        return CHOICE_PAPER;
    }
    if (strcmp(buf, "도마뱀") == 0) {
        return CHOICE_LIZARD;
    }
    if (strcmp(buf, "스팍") == 0 || strcmp(buf, "스포크") == 0) {
        return CHOICE_SPOCK;
    }
    
    return CHOICE_INVALID;
}
