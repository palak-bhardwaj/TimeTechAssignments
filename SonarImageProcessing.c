#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10
#define MAX_PIXEL_VALUE 256

void printMatrix(int *mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%3d  ", *(mat + i * n + j));
        }
        printf("\n");
    }
    printf("\n");
}

void transposeMatrix(int *mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int temp = *(mat + i * n + j);
            *(mat + i * n + j) = *(mat + j * n + i);
            *(mat + j * n + i) = temp;
        }
    }
}

void reverseRows(int *mat, int n) {
    for (int i = 0; i < n; i++) {
        int start = 0, end = n - 1;
        while (start < end) {
            int temp = *(mat + i * n + start);
            *(mat + i * n + start) = *(mat + i * n + end);
            *(mat + i * n + end) = temp;
            start++;
            end--;
        }
    }
}

void rotateMatrix(int *mat, int n) {
    transposeMatrix(mat, n);
    reverseRows(mat, n);
}

int calculateAverage(int *mat, int n, int i, int j) {
    int sum = 0, count = 0;
    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            int newI = i + di;
            int newJ = j + dj;
            if (newI >= 0 && newI < n && newJ >= 0 && newJ < n) {
                sum += *(mat + newI * n + newJ);
                count++;
            }
        }
    }
    return sum / count;
}

void smoothMatrix(int *mat, int n) {
    int tempRow[MAX];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            tempRow[j] = calculateAverage(mat, n, i, j);
        }
        for (int j = 0; j < n; j++) {
            *(mat + i * n + j) = tempRow[j];
        }
    }
}

void sonarProcessing() {
    int n;
    printf("Enter matrix size (2–10): ");
    scanf("%d", &n);

    if (n < 2 || n > MAX) {
        printf("Invalid size!.Enter a value between 2 and 10.\n");
        return;
    }

    int matrix[MAX][MAX];
    srand(time(0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % MAX_PIXEL_VALUE;
        }
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix((int *)matrix, n);

    rotateMatrix((int *)matrix, n);
    printf("Matrix after 90° Clockwise Rotation:\n");
    printMatrix((int *)matrix, n);

    smoothMatrix((int *)matrix, n);
    printf("Matrix after Applying 3x3 Smoothing Filter:\n");
    printMatrix((int *)matrix, n);
}

int main() {
    sonarProcessing();
    return 0;
}
