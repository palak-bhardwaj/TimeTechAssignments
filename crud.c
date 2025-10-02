#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define FILENAME "users.txt"
#define TEMP_FILENAME "temp.txt"

typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
} User;

int get_next_id() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 1;
    int max_id = 0;
    User u;
    while (fscanf(file, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3)
        if (u.id > max_id) max_id = u.id;
    fclose(file);
    return max_id + 1;
}

void add_user() {
    FILE *file = fopen(FILENAME, "a");
    if (!file) { printf("Cannot open file.\n"); return; }

    User u;
    u.id = get_next_id();
    printf("ID: %d\nName: ", u.id);
    scanf("%s", u.name);
    printf("Age: ");
    scanf("%d", &u.age);

    fprintf(file, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(file);
    printf("User added.\n");
}

void read_users() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) { printf("No users found.\n"); return; }

    User u;
    while (fscanf(file, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3)
        printf("ID: %d, Name: %s, Age: %d\n", u.id, u.name, u.age);

    fclose(file);
}

void update_user() {
    int id;
    printf("Enter ID to update: ");
    if (scanf("%d", &id) != 1) { while(getchar()!='\n'); return; }

    FILE *orig = fopen(FILENAME, "r");
    FILE *temp = fopen(TEMP_FILENAME, "w");
    if (!orig || !temp) { if(orig) fclose(orig); if(temp) fclose(temp); return; }

    User u; int found = 0;
    while (fscanf(orig, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            found = 1;
            printf("New Name: ");
            scanf("%s", u.name);
            printf("New Age: ");
            scanf("%d", &u.age);
        }
        fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(orig); fclose(temp);
    if (found) { remove(FILENAME); rename(TEMP_FILENAME, FILENAME); printf("Updated.\n"); }
    else remove(TEMP_FILENAME);
}

void delete_user() {
    int id;
    printf("Enter ID to delete: ");
    if (scanf("%d", &id) != 1) { while(getchar()!='\n'); return; }

    FILE *orig = fopen(FILENAME, "r");
    FILE *temp = fopen(TEMP_FILENAME, "w");
    if (!orig || !temp) { if(orig) fclose(orig); if(temp) fclose(temp); return; }

    User u; int found = 0;
    while (fscanf(orig, "%d,%[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) found = 1;
        else fprintf(temp, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(orig); fclose(temp);
    if (found) { remove(FILENAME); rename(TEMP_FILENAME, FILENAME); printf("Deleted.\n"); }
    else remove(TEMP_FILENAME);
}

int main() {
    int choice;
    do {
        printf("\n1. Add\n2. Show\n3. Update\n4. Delete\n0. Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) { while(getchar()!='\n'); choice=-1; continue; }
        switch(choice){
            case 1: add_user(); break;
            case 2: read_users(); break;
            case 3: update_user(); break;
            case 4: delete_user(); break;
            case 0: break;
            default: printf("Invalid.\n");
        }
    } while(choice != 0);
    return 0;
}
