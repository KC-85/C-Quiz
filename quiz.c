#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // For toupper()
#include <time.h>   // For time() function (random number seeding)
#include "cJSON.h"  // Include the cJSON library

#define MAX_LENGTH 300
#define MAX_QUESTIONS 1000
#define QUESTIONS_TO_ASK 10  // Number of questions to ask in each game

// Structure to hold a quiz question with multiple choices
struct Question {
    char question[MAX_LENGTH];
    char choices[4][MAX_LENGTH];  // 4 multiple choice options
    char correctAnswer;           // The correct answer (A, B, C, D)
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
        struct Question temp = questions[i];
        questions[i] = questions[j];
        questions[j] = temp;
    }
}

// Function to load questions from a JSON file with more detailed error handling
int loadQuestionsFromJSON(const char *filename, struct Question *questions) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        return 0;
    }

    // Read the entire file into memory
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    if (fileSize <= 0) {
        printf("Error: Empty or invalid file %s\n", filename);
        fclose(file);
        return 0;
    }

    char *fileContents = (char *)malloc(fileSize + 1);
    fread(fileContents, 1, fileSize, file);
    fclose(file);
    fileContents[fileSize] = '\0';

    // Parse the JSON file
    cJSON *json = cJSON_Parse(fileContents);
    if (json == NULL) {
        printf("Error parsing JSON.\n");
        free(fileContents);
        return 0;
    }

    // Parse the questions array
    cJSON *questionsArray = cJSON_GetObjectItem(json, "questions");
    if (!questionsArray) {
        printf("Error: No questions found in JSON file.\n");
        free(fileContents);
        cJSON_Delete(json);
        return 0;
    }

    int count = cJSON_GetArraySize(questionsArray);
    if (count == 0) {
        printf("Error: No valid questions found.\n");
        free(fileContents);
        cJSON_Delete(json);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        cJSON *questionObj = cJSON_GetArrayItem(questionsArray, i);
        cJSON *questionText = cJSON_GetObjectItem(questionObj, "question");
        cJSON *choices = cJSON_GetObjectItem(questionObj, "choices");
        cJSON *correctAnswer = cJSON_GetObjectItem(questionObj, "correctAnswer");

        if (!questionText || !choices || !correctAnswer) {
            printf("Error: Malformed question or missing data at question %d.\n", i + 1);
            continue;
        }

        strcpy(questions[i].question, questionText->valuestring);

        for (int j = 0; j < 4; j++) {
            strcpy(questions[i].choices[j], cJSON_GetArrayItem(choices, j)->valuestring);
        }

        questions[i].correctAnswer = correctAnswer->valuestring[0];
    }

    free(fileContents);
    cJSON_Delete(json);

    return count;  // Return the number of questions loaded
}

// Function to ask a question and check the answer
int askQuestion(struct Question q) {
    char userAnswer;

    printf("%s\n", q.question);
    printf("A. %s\n", q.choices[0]);
    printf("B. %s\n", q.choices[1]);
    printf("C. %s\n", q.choices[2]);
    printf("D. %s\n", q.choices[3]);
    printf("Your answer (A/B/C/D): ");
    scanf(" %c", &userAnswer);

    // Convert to uppercase for comparison
    if (toupper(userAnswer) == q.correctAnswer) {
        printCorrectAnswer();
        return 1;  // Correct answer
    } else {
        printIncorrectAnswer();
        printf("The correct answer was: %c\n\n", q.correctAnswer);
        return 0;  // Incorrect answer
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

    struct Question questions[MAX_QUESTIONS];
    int questionCount = loadQuestionsFromJSON("questions.json", questions);

    if (questionCount > 0) {
        runQuiz(questions, questionCount);
    } else {
        printf("Error: Unable to load questions. Exiting.\n");
        return 1;
    }

    return 0;
}
