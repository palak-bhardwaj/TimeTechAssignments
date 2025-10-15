#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10

void PrintMatrix(int *mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++){
            printf("%3d  ", *(mat + i*n + j));
        }
        printf("\n");
    }
    printf("\n");
}

void TransposeMatrix(int *mat, int n) {
    for (int i = 0; i < n; i++){
        for (int j = i + 1; j < n; j++){
            int temp = *(mat + i*n + j);
            *(mat + i*n + j) = *(mat + j*n + i);
            *(mat + j*n + i) = temp;
        }
    }
}

void ReverseRows(int *mat, int n) {
    for (int i = 0; i < n; i++) {
        int start = 0, end = n - 1;
        while (start < end) {
            int temp = *(mat + i*n + start);
            *(mat + i*n + start) = *(mat + i*n + end);
            *(mat + i*n + end) = temp;
            start++;
            end--;
        }
    }
}

void RotateMatrix(int *mat, int n) {
    TransposeMatrix(mat, n);
    ReverseRows(mat, n);
}

void SmoothMatrix(int *mat, int n) {
    int tempRow[MAX];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            int count = 0;

            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int newi = i + di;
                    int newj = j + dj;
                    if (newi >= 0 && newi < n && newj >= 0 && newj < n){
                        sum += *(mat + newi*n + newj);
                        count++;
                    }
                }
            }
            tempRow[j] = sum / count;
        }

        for (int j = 0; j < n; j++) {
            *(mat + i*n + j) = tempRow[j];
        }
    }
}

int main() 
{
    int n;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);

    int matrix[MAX][MAX];
    srand(time(0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 256;
        }
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    PrintMatrix((int *)matrix, n);

    RotateMatrix((int *)matrix, n);
    printf("Matrix after 90° Clockwise Rotation:\n");
    PrintMatrix((int *)matrix, n);

    SmoothMatrix((int *)matrix, n);
    printf("Matrix after Applying 3x3 Smoothing Filter:\n");
    PrintMatrix((int *)matrix, n);

    return 0;
}
