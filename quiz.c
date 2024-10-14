#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>  // For toupper()

#define MAX_LENGTH 300
#define EASY_QUESTIONS 1000
#define MEDIUM_QUESTIONS 1000
#define HARD_QUESTIONS 1000
#define EXTREME_QUESTIONS 1000
#define QUESTIONS_TO_ASK 10  // Number of questions to ask in each game

// Structure to hold a quiz question with multiple choices
struct Question {
    char question[MAX_LENGTH];
    char choices[4][MAX_LENGTH];  // 4 multiple choice options
    char correctAnswer;  // The correct answer (A, B, C, D)
};

// Function to display the ASCII game banner
void printGameBanner() {
    printf("**********************************************\n");
    printf("*                                            *\n");
    printf("*            WELCOME TO THE QUIZ GAME         *\n");
    printf("*                                            *\n");
    printf("**********************************************\n");
    printf("           Test your knowledge now!           \n");
    printf("\n");
}

// Function to display text for correct answers
void printCorrectAnswer() {
    printf("Correct!\n\n");
}

// Function to display text for incorrect answers
void printIncorrectAnswer() {
    printf("Incorrect!\n\n");
}

// Function to shuffle questions using Fisher-Yates shuffle algorithm
void shuffleQuestions(struct Question *questions, int count) {
    srand(time(NULL));  // Seed the random number generator
    for (int i = count - 1; i > 0; i--) {
        int j = rand() % (i + 1);  // Get a random index
        // Swap questions[i] with questions[j]
        struct Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

// Function to load questions from a file based on difficulty level
int loadQuestions(const char *filename, struct Question *questions, int maxQuestions, const char *difficulty) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char line[MAX_LENGTH];
    int count = 0;
    int load = 0;  // Indicator for loading questions based on difficulty
    while (fgets(line, MAX_LENGTH, file) != NULL && count < maxQuestions) {
        if (strstr(line, difficulty) != NULL) {
            load = 1;  // Start loading questions for this difficulty
            continue;
        }

        if (line[0] == '#') {
            load = 0;  // Stop loading when a different difficulty block starts
        }

        if (load) {
            if (fgets(questions[count].question, MAX_LENGTH, file) == NULL ||
                fgets(questions[count].choices[0], MAX_LENGTH, file) == NULL ||
                fgets(questions[count].choices[1], MAX_LENGTH, file) == NULL ||
                fgets(questions[count].choices[2], MAX_LENGTH, file) == NULL ||
                fgets(questions[count].choices[3], MAX_LENGTH, file) == NULL) {
                printf("Error reading question %d from file.\n", count + 1);
                break;
            }

            // Read correct answer
            char answerLine[3];  // To handle newline after the correct answer
            if (fgets(answerLine, sizeof(answerLine), file) == NULL) {
                printf("Error reading answer for question %d.\n", count + 1);
                break;
            }
            questions[count].correctAnswer = answerLine[0];  // First char is the answer

            count++;
        }
    }

    fclose(file);
    return count;  // Return the number of questions loaded
}

// Function to dynamically allocate memory for questions
struct Question* allocateQuestions(int numQuestions) {
    struct Question* questions = malloc(numQuestions * sizeof(struct Question));
    if (questions == NULL) {
        perror("Memory allocation failed");
        exit(1);  // Exit if memory allocation fails
    }
    return questions;
}

// Function to ask a question and check the answer
int askQuestion(struct Question q) {
    char userAnswer;

    // Display the question and answer choices
    printf("%s", q.question);
    printf("A. %s", q.choices[0]);
    printf("B. %s", q.choices[1]);
    printf("C. %s", q.choices[2]);
    printf("D. %s", q.choices[3]);

    // Input loop to ensure valid input
    do {
        printf("Your answer (A/B/C/D): ");
        scanf(" %c", &userAnswer);
        userAnswer = toupper(userAnswer);  // Convert to uppercase

        // Check for valid input
        if (userAnswer < 'A' || userAnswer > 'D') {
            printf("Invalid input. Please enter A, B, C, or D.\n");
        }
    } while (userAnswer < 'A' || userAnswer > 'D');  // Continue until valid input

    // Check if the user's answer is correct
    if (userAnswer == toupper(q.correctAnswer)) {
        printCorrectAnswer();
        return 1;  // Correct
    } else {
        printIncorrectAnswer();
        printf("The correct answer was: %c. %s", q.correctAnswer, q.choices[q.correctAnswer - 'A']);
        return 0;  // Incorrect
    }
}

// Function to display the final score
void displayFinalScore(int score, int totalQuestions) {
    printf("\nYour total score: %d out of %d\n", score, totalQuestions);
    float percentage = ((float)score / totalQuestions) * 100;
    printf("That's a score of %.2f%%\n", percentage);

    // Feedback based on performance
    if (percentage >= 90) {
        printf("Excellent! You're a quiz master!\n");
    } else if (percentage >= 70) {
        printf("Good job! Keep learning!\n");
    } else {
        printf("You might want to study more. Better luck next time!\n");
    }
}

// Function to run the quiz game
void runQuiz(struct Question *questions, int questionCount) {
    int score = 0;

    // Shuffle the questions
    shuffleQuestions(questions, questionCount);

    // Ask a subset of questions
    for (int i = 0; i < QUESTIONS_TO_ASK && i < questionCount; i++) {
        score += askQuestion(questions[i]);
    }

    // Display the final score
    displayFinalScore(score, QUESTIONS_TO_ASK);
}

int main() {
    // Display the game introduction banner
    printGameBanner();

    // Dynamically allocate memory for each difficulty level
    struct Question* easyQuestions = allocateQuestions(EASY_QUESTIONS);
    struct Question* mediumQuestions = allocateQuestions(MEDIUM_QUESTIONS);
    struct Question* hardQuestions = allocateQuestions(HARD_QUESTIONS);
    struct Question* extremeQuestions = allocateQuestions(EXTREME_QUESTIONS);

    // Load questions for each difficulty level
    int easyCount = loadQuestions("questions.txt", easyQuestions, EASY_QUESTIONS, "#Easy");
    int mediumCount = loadQuestions("questions.txt", mediumQuestions, MEDIUM_QUESTIONS, "#Medium");
    int hardCount = loadQuestions("questions.txt", hardQuestions, HARD_QUESTIONS, "#Hard");
    int extremeCount = loadQuestions("questions.txt", extremeQuestions, EXTREME_QUESTIONS, "#Extreme");

    // Ensure that questions are loaded properly
    if (easyCount == 0 || mediumCount == 0 || hardCount == 0 || extremeCount == 0) {
        printf("Error loading questions. Exiting.\n");
        free(easyQuestions);
        free(mediumQuestions);
        free(hardQuestions);
        free(extremeQuestions);
        return 1;
    }

    // Example: Run the quiz with easy questions (replace this with actual user input logic)
    runQuiz(easyQuestions, easyCount);

    // Free allocated memory after use
    free(easyQuestions);
    free(mediumQuestions);
    free(hardQuestions);
    free(extremeQuestions);

    return 0;
}
