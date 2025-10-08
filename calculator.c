#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define SIZE 200  

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

bool isValidExpression(const char expr[]) {
    for (int i = 0; expr[i] != '\0'; i++) {
        unsigned char ch = (unsigned char) expr[i];
        if (!(isdigit(ch) || isOperator(ch) || isspace(ch))) {
            return false;
        }
    }
    return true;
}

int evaluateExpression(const char expr[], int *errorCode) {
    int numbers[SIZE];
    char operators[SIZE];
    int numCount = 0, opCount = 0, index = 0;

    while (expr[index] != '\0') {
        unsigned char ch = (unsigned char) expr[index];
        
        if (isspace(ch)) {
            index++;
            continue;
        }

        if (isdigit(ch)) {
            int value = 0;
            while (isdigit((unsigned char) expr[index])) {
                value = value * 10 + (expr[index] - '0');
                index++;
            }
            numbers[numCount++] = value;
        } 
        else if (isOperator(ch)) {
            operators[opCount++] = expr[index];
            index++;
        } 
        else {
            *errorCode = 1;
            return 0;
        }
    }

    for (int j = 0; j < opCount; j++) {
        if (operators[j] == '*' || operators[j] == '/') {
            int a = numbers[j];
            int b = numbers[j + 1];

            if (operators[j] == '/' && b == 0) {
                *errorCode = 2;
                return 0;
            }

            int result;
            if (operators[j] == '*') {
                result = a * b;
            } else {
                result = a / b;
            }

            numbers[j] = result;

            for (int k = j + 1; k < numCount - 1; k++) {
                numbers[k] = numbers[k + 1];
            }
            for (int k = j; k < opCount - 1; k++) {
                operators[k] = operators[k + 1];
            }
            numCount--;
            opCount--;
            j--;
        }
    }

    int answer = numbers[0];
    for (int j = 0; j < opCount; j++) {
        if (operators[j] == '+') {
            answer += numbers[j + 1];
        } else if (operators[j] == '-') {
            answer -= numbers[j + 1];
        }
    }

    return answer;
}

void calculator() {
    char input[SIZE];
    printf("Enter an expression: ");
    if (!fgets(input, SIZE, stdin)) {
        printf("Error reading input.\n");
        return;
    }

    if (!isValidExpression(input)) {
        printf("Error: Invalid expression.\n");
        return;
    }

    int errorCode = 0;
    int result = evaluateExpression(input, &errorCode);

    if (errorCode == 1) {
        printf("Error: Invalid expression.\n");
    } 
    else if (errorCode == 2) {
        printf("Error: Division by zero.\n");
    } 
    else {
        printf("Result: %d\n", result);
    }
}

int main() {
    Calculator();
    return 0;
}
