// 가위, 바위, 보, 도마뱀, 스팍 게임 프로그램
// Rock-Paper-Scissors-Lizard-Spock 게임
// 규칙:
// - Rock crushes Scissors and crushes Lizard
// - Paper covers Rock and disproves Spock
// - Scissors cuts Paper and decapitates Lizard
// - Lizard eats Paper and poisons Spock
// - Spock vaporizes Rock and smashes Scissors
// 사용자가 이기면 "You win!", 지면 "You lose!", 비기면 "It's a tie!"를 출력합니다.
// 사용자가에게 다시 게임을 할지 물어보고,(y/n)을 입력받는다.
// 사용자가 'y'를 입력하면 게임을 다시 시작하고, 'n'을 입력하면 프로그램을 종료합니다.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define ROCK 0
#define SCISSORS 1
#define PAPER 2
#define LIZARD 3
#define SPOCK 4

const char* choices[] = {"Rock", "Scissors", "Paper", "Lizard", "Spock"};
int getComputerChoice() {
    return rand() % 5; // 0, 1, 2, 3, 4 중 하나를 무작위로 선택
}

int determineWinner(int userChoice, int computerChoice) {
    if (userChoice == computerChoice) {
        return 0; // 비김
    }
    
    // 사용자가 이기는 경우들
    if ((userChoice == ROCK && (computerChoice == SCISSORS || computerChoice == LIZARD)) ||
        (userChoice == PAPER && (computerChoice == ROCK || computerChoice == SPOCK)) ||
        (userChoice == SCISSORS && (computerChoice == PAPER || computerChoice == LIZARD)) ||
        (userChoice == LIZARD && (computerChoice == PAPER || computerChoice == SPOCK)) ||
        (userChoice == SPOCK && (computerChoice == ROCK || computerChoice == SCISSORS))) {
        return 1; // 사용자 승리
    } else {
        return -1; // 컴퓨터 승리
    }
}
int main() {
    char userInput[10];
    srand((unsigned int)time(NULL)); // 난수 초기화

    while (1) {
        printf("Enter your choice (rock, scissors, paper, lizard, spock): ");
        scanf("%s", userInput);

        int userChoice;
        if (strcmp(userInput, "rock") == 0) {
            userChoice = ROCK;
        } else if (strcmp(userInput, "scissors") == 0) {
            userChoice = SCISSORS;
        } else if (strcmp(userInput, "paper") == 0) {
            userChoice = PAPER;
        } else if (strcmp(userInput, "lizard") == 0) {
            userChoice = LIZARD;
        } else if (strcmp(userInput, "spock") == 0) {
            userChoice = SPOCK;
        } else {
            printf("Invalid choice. Please try again.\n");
            continue; // 잘못된 입력 시 다시 입력 받기
        }

        int computerChoice = getComputerChoice();
        printf("Computer chose: %s\n", choices[computerChoice]);

        int result = determineWinner(userChoice, computerChoice);
        if (result == 1) {
            printf("You win!\n");
        } else if (result == -1) {
            printf("You lose!\n");
        } else {
            printf("It's a tie!\n");
        }

        while (1) {
            printf("Do you want to play again? (y/n): ");
            scanf("%s", userInput);
            if (strcmp(userInput, "y") == 0) {
                break; // 'y' 입력 시 게임 재시작 (while 루프 계속)
            } else if (strcmp(userInput, "n") == 0) {
                return 0; // 'n' 입력 시 프로그램 종료
            } else {
                printf("Invalid input. Please enter 'y' or 'n'.\n");
            }
        }
    }

    return 0;
}