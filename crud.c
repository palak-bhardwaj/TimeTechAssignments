#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME 50
#define FILENAME "users.txt"
#define TEMP_FILENAME "temp.txt"

typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
} User;

int getNextId() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        return 1;
    }

    int maxId = 0;
    User u;
    while (fscanf(file, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id > maxId) {
            maxId = u.id;
        }
    }
    fclose(file);
    return maxId + 1;
}

void addUser() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Cannot open file.\n");
        return;
    }

    User u;
    u.id = getNextId();
    printf("ID: %d\nName: ", u.id);
    scanf("%s", u.name);
    printf("Age: ");
    scanf("%d", &u.age);

    fprintf(file, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(file);
    printf("User added.\n");
}

void readUsers() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No users found.\n");
        return;
    }

    User u;
    while (fscanf(file, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        printf("ID: %d, Name: %s, Age: %d\n", u.id, u.name, u.age);
    }

    fclose(file);
}

void updateUser() {
    int id;
    printf("Enter ID to update: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        return;
    }

    FILE *orig = fopen(FILENAME, "r");
    FILE *temp = fopen(TEMP_FILENAME, "w");
    if (!orig || !temp) {
        if (orig) fclose(orig);
        if (temp) fclose(temp);
        return;
    }

    User u;
    bool found = false;
    while (fscanf(orig, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            found = true;
            printf("New Name: ");
            scanf("%s", u.name);
            printf("New Age: ");
            scanf("%d", &u.age);
        }
        fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(orig);
    fclose(temp);

    if (found) {
        remove(FILENAME);
        rename(TEMP_FILENAME, FILENAME);
        printf("Updated.\n");
    } 
    else {
        remove(TEMP_FILENAME);
    }
}

void deleteUser() {
    int id;
    printf("Enter ID to delete: ");
    if (scanf("%d", &id) != 1) {
        while (getchar() != '\n');
        return;
    }

    FILE *orig = fopen(FILENAME, "r");
    FILE *temp = fopen(TEMP_FILENAME, "w");
    if (!orig || !temp) {
        if (orig) fclose(orig);
        if (temp) fclose(temp);
        return;
    }

    User u;
    bool found = false;
    while (fscanf(orig, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            found = true;
        } 
        else {
            fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
        }
    }

    fclose(orig);
    fclose(temp);

    if (found) {
        remove(FILENAME);
        rename(TEMP_FILENAME, FILENAME);
        printf("Deleted.\n");
    } 
    else {
        remove(TEMP_FILENAME);
    }
}

void crudApp() {
    int choice;
    do {
        printf("\n1. Add\n2. Show\n3. Update\n4. Delete\n0. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
            continue;
        }
        switch (choice) {
            case 1: addUser(); break;
            case 2: readUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while (choice != 0);
}

int main() {
    crudApp();
    return 0;
}
