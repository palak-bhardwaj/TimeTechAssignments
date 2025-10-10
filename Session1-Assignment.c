#include <stdio.h>
#include <string.h>

struct Student {
    int rollNumber;
    char name[50];
    float marks1, marks2, marks3;
    float totalMarks;
    float averageMarks;
    char grade;
};

int totalStudents;

float calculateTotal(float m1, float m2, float m3) {
    return m1 + m2 + m3;
}

float calculateAverage(float total) {
    return total / 3.0;
}

char assignGrade(float average) {
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

void displayPerformance(char grade, float average) {
    int i, stars = 0;
    switch (grade) {
        case 'A': stars = 5; break;
        case 'B': stars = 4; break;
        case 'C': stars = 3; break;
        case 'D': stars = 2; break;
        default: stars = 0;
    }
    if (average < 35) return;
    for (i = 0; i < stars; i++)
        printf("*");
}

void calculateResults(struct Student *s) {
    s->totalMarks = calculateTotal(s->marks1, s->marks2, s->marks3);
    s->averageMarks = calculateAverage(s->totalMarks);
    s->grade = assignGrade(s->averageMarks);
}

void inputStudents(struct Student students[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        printf("\nEnter details for student %d:\n", i + 1);
        printf("Roll Number: ");
        scanf("%d", &students[i].rollNumber);
        printf("Name: ");
        scanf("%s", students[i].name);
        printf("Enter marks in 3 subjects: ");
        scanf("%f %f %f", &students[i].marks1, &students[i].marks2, &students[i].marks3);
        calculateResults(&students[i]);
    }
}

void displayStudents(struct Student students[], int n) {
    int i;
    printf("\n---------- Student Performance ----------\n");
    for (i = 0; i < n; i++) {
        printf("\nRoll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %.2f\n", students[i].totalMarks);
        printf("Average: %.2f\n", students[i].averageMarks);
        printf("Grade: %c\n", students[i].grade);
        if (students[i].averageMarks < 35)
            continue;
        printf("Performance: ");
        displayPerformance(students[i].grade, students[i].averageMarks);
        printf("\n");
    }
}

void printRollNumbersRecursively(int current, int total) {
    if (current == total) return;
    printf("%d ", current + 1);
    printRollNumbersRecursively(current + 1, total);
}

int main() {
    printf("Enter number of students: ");
    scanf("%d", &totalStudents);
    if (totalStudents < 1 || totalStudents > 100) {
        printf("Invalid number of students!\n");
        return 0;
    }
    struct Student students[totalStudents];
    inputStudents(students, totalStudents);
    displayStudents(students, totalStudents);
    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbersRecursively(0, totalStudents);
    printf("\n");
    return 0;
}
