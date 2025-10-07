#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define SIZE 200  

int isOp(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
}

int valid(char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (!(isdigit(s[i]) || isOp(s[i]) || isspace(s[i]))) {
            return 0;
        }
    }
    return 1;
}

int eval(char s[], int *err) {
    int nums[SIZE];
    char ops[SIZE];
    int n = 0, o = 0, i = 0;

    while (s[i] != '\0') {
        if (isspace(s[i])) {
            i++;
            continue;
        }
        if (isdigit(s[i])) {
            int val = 0;
            while (isdigit(s[i])) {
                val = val * 10 + (s[i] - '0');
                i++;
            }
            nums[n++] = val;
        } 
        else if (isOp(s[i])) {
            ops[o++] = s[i];
            i++;
        } 
        else {
            *err = 1;
            return 0;
        }
    }

    for (int j = 0; j < o; j++) {
        if (ops[j] == '*' || ops[j] == '/') {
            int a = nums[j];
            int b = nums[j + 1];

            if (ops[j] == '/' && b == 0) {
                *err = 2;
                return 0;
            }

            int r;
            if (ops[j] == '*')
                r = a * b;
            else
                r = a / b;

            nums[j] = r;

            for (int k = j + 1; k < n - 1; k++) {
                nums[k] = nums[k + 1];
            }
            for (int k = j; k < o - 1; k++) {
                ops[k] = ops[k + 1];
            }
            n--;
            o--;
            j--;
        }
    }

    int ans = nums[0];
    for (int j = 0; j < o; j++) {
        if (ops[j] == '+')
            ans += nums[j + 1];
        else if (ops[j] == '-')
            ans -= nums[j + 1];
    }

    return ans;
}

int main() {
    char in[SIZE];
    printf("Enter an expression: ");
    fgets(in, SIZE, stdin);

    if (!valid(in)) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    int e = 0;
    int res = eval(in, &e);

    if (e == 1) {
        printf("Error: Invalid expression.\n");
    } 
    else if (e == 2) {
        printf("Error: Division by zero.\n");
    } 
    else {
        printf("%d\n", res);
    }

    return 0;
}
