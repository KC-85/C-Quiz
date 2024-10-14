#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
        printf("Error: Unable to open the file %s\n", filename);
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
            fgets(questions[count].question, MAX_LENGTH, file);
            fgets(questions[count].choices[0], MAX_LENGTH, file);  // Option A
            fgets(questions[count].choices[1], MAX_LENGTH, file);  // Option B
            fgets(questions[count].choices[2], MAX_LENGTH, file);  // Option C
            fgets(questions[count].choices[3], MAX_LENGTH, file);  // Option D
            questions[count].correctAnswer = fgetc(file);  // Read the correct answer

            fgetc(file);  // Consume the newline after the correct answer
            count++;
        }
    }

    fclose(file);
    return count;  // Return the number of questions loaded
}

// Function to ask a question and check the answer
int askQuestion(struct Question q) {
    char userAnswer;
    printf("%s", q.question);
    printf("%s", q.choices[0]);  // Display choice A
    printf("%s", q.choices[1]);  // Display choice B
    printf("%s", q.choices[2]);  // Display choice C
    printf("%s", q.choices[3]);  // Display choice D
    printf("Your answer (A/B/C/D): ");
    scanf(" %c", &userAnswer);

    // Check if the user's answer is correct
    if (userAnswer == q.correctAnswer) {
        printCorrectAnswer();
        return 1;  // Return 1 if the answer is correct
    } else {
        printIncorrectAnswer();
        // Find the correct choice and reveal the full answer text
        printf("The correct answer was: ");
        switch (q.correctAnswer) {
            case 'A':
                printf("%s\n", q.choices[0]);  // Reveal option A
                break;
            case 'B':
                printf("%s\n", q.choices[1]);  // Reveal option B
                break;
            case 'C':
                printf("%s\n", q.choices[2]);  // Reveal option C
                break;
            case 'D':
                printf("%s\n", q.choices[3]);  // Reveal option D
                break;
        }
        printf("\n");
        return 0;  // Return 0 if the answer is incorrect
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
    printf("Your total score: %d out of %d\n", score, QUESTIONS_TO_ASK);
}

int main() {
    // Display the game introduction banner
    printGameBanner();

    // Define variables and initialize quiz data
    struct Question easyQuestions[EASY_QUESTIONS];
    struct Question mediumQuestions[MEDIUM_QUESTIONS];
    struct Question hardQuestions[HARD_QUESTIONS];
    struct Question extremeQuestions[EXTREME_QUESTIONS];

    // Load questions for each difficulty level
    int easyCount = loadQuestions("questions.txt", easyQuestions, EASY_QUESTIONS, "#Easy");
    int mediumCount = loadQuestions("questions.txt", mediumQuestions, MEDIUM_QUESTIONS, "#Medium");
    int hardCount = loadQuestions("questions.txt", hardQuestions, HARD_QUESTIONS, "#Hard");
    int extremeCount = loadQuestions("questions.txt", extremeQuestions, EXTREME_QUESTIONS, "#Extreme");

    if (easyCount == 0 || mediumCount == 0 || hardCount == 0 || extremeCount == 0) {
        printf("Error loading questions. Exiting.\n");
        return 1;
    }

    // Example: Run the quiz with easy questions (replace this with actual user input logic)
    runQuiz(easyQuestions, easyCount);

    return 0;
}
