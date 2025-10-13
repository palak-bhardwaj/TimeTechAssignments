#include <stdio.h>
#include <stdbool.h>

#define MAX_SUBJECTS 3
#define MAX_STUDENTS 100
#define GRADE_A 85
#define GRADE_B 70
#define GRADE_C 50
#define GRADE_D 35

typedef enum { A = 'A', B = 'B', C = 'C', D = 'D', F = 'F' } Grade;

typedef struct {
    int rollNumber;
    char name[50];
    float marks[MAX_SUBJECTS];
} Student;

bool isUniqueRollNumber(Student students[], int numberOfStudents, int rollNumber) {
    for (int i = 0; i < numberOfStudents; i++) {
        if (students[i].rollNumber == rollNumber) {
            return false;
        }
    }
    return true;
}

bool isValidMarks(float marks) {
    return marks >= 0 && marks <= 100;
}

float calculateTotal(Student s) {
    float total = 0;
    for (int i = 0; i < MAX_SUBJECTS; i++) {
        total += s.marks[i];
    }
    return total;
}

float calculateAverage(Student s) {
    return calculateTotal(s) / MAX_SUBJECTS;
}

Grade assignGrade(float avg) {
    if (avg >= GRADE_A) {
        return A;
    } 
    else if (avg >= GRADE_B) {
        return B;
    } 
    else if (avg >= GRADE_C) {
        return C;
    } 
    else if (avg >= GRADE_D) {
        return D;
    } 
    else {
        return F;
    }
}

void inputStudents(Student students[], int n) {
    for (int i = 0; i < n; i++) {
        scanf("%d", &students[i].rollNumber);
        while (!isUniqueRollNumber(students, i, students[i].rollNumber)) {
            scanf("%d", &students[i].rollNumber);
        }

        scanf("%s", students[i].name);

        for (int j = 0; j < MAX_SUBJECTS; j++) {
            scanf("%f", &students[i].marks[j]);
            while (!isValidMarks(students[i].marks[j])) {
                scanf("%f", &students[i].marks[j]);
            }
        }
    }
}

void sortByRoll(Student students[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (students[i].rollNumber > students[j].rollNumber) {
                Student temp = students[i];
                students[i] = students[j];
                students[j] = temp;
            }
        }
    }
}

void printPerformance(Grade grade) {
    int stars = 0;
    switch (grade) {
        case A: stars = 5; break;
        case B: stars = 4; break;
        case C: stars = 3; break;
        case D: stars = 2; break;
        default: stars = 0; break;
    }

    if (grade == F) {
        return;
    }

    for (int i = 0; i < stars; i++) {
        printf("*");
    }
}

void displayStudents(Student students[], int n) {
    for (int i = 0; i < n; i++) {
        float total = calculateTotal(students[i]);
        float avg = calculateAverage(students[i]);
        Grade grade = assignGrade(avg);

        printf("Roll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %.0f\n", total);
        printf("Average: %.2f\n", avg);
        printf("Grade: %c\n", grade);

        if (grade == F) {
            printf("Performance:\n");
            continue;
        } 
        else {
            printf("Performance: ");
            printPerformance(grade);
            printf("\n");
        }
    }
}

void printRollNumbers(Student students[], int n, int index) {
    if (index == n) {
        return;
    } else {
        printf("%d ", students[index].rollNumber);
        printRollNumbers(students, n, index + 1);
    }
}

int main() {
    int totalStudents;
    scanf("%d", &totalStudents);

    if (totalStudents < 1 || totalStudents > MAX_STUDENTS) {
        return 0;
    }

    Student students[MAX_STUDENTS];
    inputStudents(students, totalStudents);
    sortByRoll(students, totalStudents);
    displayStudents(students, totalStudents);

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, totalStudents, 0);
    printf("\n");

    return 0;
}
