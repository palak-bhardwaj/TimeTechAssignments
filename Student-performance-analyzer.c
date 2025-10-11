#include <stdio.h>

#define MAX_SUBJECTS 3
#define MAX_STUDENTS 100
#define GRADE_A 85
#define GRADE_B 70
#define GRADE_C 50
#define GRADE_D 35

enum Grade { A, B, C, D, F };

struct Student {
    int rollNumber;
    char name[50];
    float marks[MAX_SUBJECTS];
};

int isUniqueRoll(struct Student students[], int n, int roll) {
    for (int i = 0; i < n; i++)
        if (students[i].rollNumber == roll)
            return 0;
    return 1;
}

int isValidMarks(float marks) {
    return marks >= 0 && marks <= 100;
}

float calculateAverage(struct Student s) {
    float total = 0;
    for (int i = 0; i < MAX_SUBJECTS; i++)
        total += s.marks[i];
    return total / MAX_SUBJECTS;
}

enum Grade assignGrade(float avg) {
    if (avg >= GRADE_A) return A;
    else if (avg >= GRADE_B) return B;
    else if (avg >= GRADE_C) return C;
    else if (avg >= GRADE_D) return D;
    else return F;
}

void inputStudents(struct Student students[], int n) {
    for (int i = 0; i < n; i++) {
        scanf("%d", &students[i].rollNumber);
        while (!isUniqueRoll(students, i, students[i].rollNumber))
            scanf("%d", &students[i].rollNumber);
        scanf("%s", students[i].name);
        for (int j = 0; j < MAX_SUBJECTS; j++) {
            scanf("%f", &students[i].marks[j]);
            while (!isValidMarks(students[i].marks[j]))
                scanf("%f", &students[i].marks[j]);
        }
    }
}

void displayStudents(struct Student students[], int n) {
    char gradeChars[] = {'A', 'B', 'C', 'D', 'F'};
    for (int i = 0; i < n; i++) {
        float avg = calculateAverage(students[i]);
        enum Grade grade = assignGrade(avg);
        printf("%d %s %.2f %c\n", students[i].rollNumber, students[i].name, avg, gradeChars[grade]);
    }
}

void printRollNumbers(struct Student students[], int n, int index) {
    if (index == n) return;
    printf("%d ", students[index].rollNumber);
    printRollNumbers(students, n, index + 1);
}

void sortByRoll(struct Student students[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (students[i].rollNumber > students[j].rollNumber) {
                struct Student temp = students[i];
                students[i] = students[j];
                students[j] = temp;
            }
        }
    }
}

int main() {
    int totalStudents;
    scanf("%d", &totalStudents);
    if (totalStudents < 1 || totalStudents > MAX_STUDENTS)
        return 0;

    struct Student students[MAX_STUDENTS];
    inputStudents(students, totalStudents);
    sortByRoll(students, totalStudents);
    displayStudents(students, totalStudents);
    printRollNumbers(students, totalStudents, 0);
    printf("\n");
    return 0;
}
