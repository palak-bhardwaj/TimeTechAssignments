#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "players_data.h"

typedef struct PlayerNode {
    int playerId;
    char name[50];
    char teamName[50];
    char role[20];
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
} PlayerNode;

typedef struct {
    int teamId;
    char name[50];
    int totalPlayers;
    float avgStrikeRate;
    PlayerNode *head;
} Team;

float computePI(const char *role, float avg, float sr, int wk, float eco) {
    if (strcmp(role, "Batsman") == 0) return (avg * sr) / 100.0;
    if (strcmp(role, "Bowler") == 0) return (wk * 2) + (100 - eco);
    return ((avg * sr) / 100.0) + (wk * 2);
}

PlayerNode* newNode(const Player *p) {
    PlayerNode *x = malloc(sizeof(PlayerNode));
    x->playerId = p->id;
    strcpy(x->name, p->name);
    strcpy(x->teamName, p->team);
    strcpy(x->role, p->role);
    x->totalRuns = p->totalRuns;
    x->battingAverage = p->battingAverage;
    x->strikeRate = p->strikeRate;
    x->wickets = p->wickets;
    x->economyRate = p->economyRate;
    x->performanceIndex = computePI(p->role, p->battingAverage, p->strikeRate, p->wickets, p->economyRate);
    x->next = NULL;
    return x;
}

void insertPlayer(Team *t, PlayerNode *p) {
    p->next = t->head;
    t->head = p;
    t->totalPlayers++;
}

void initializeTeams(Team T[]) {
    for (int i = 0; i < teamCount; i++) {
        T[i].teamId = i + 1;
        strcpy(T[i].name, teams[i]);
        T[i].totalPlayers = 0;
        T[i].avgStrikeRate = 0;
        T[i].head = NULL;
    }
}

int searchTeam(Team T[], int id) {
    int l = 0, r = teamCount - 1;
    while (l <= r) {
        int m = (l + r) / 2;
        if (T[m].teamId == id) return m;
        if (T[m].teamId < id) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

void loadInitialPlayers(Team T[]) {
    for (int i = 0; i < playerCount; i++) {
        for (int j = 0; j < teamCount; j++) {
            if (strcmp(players[i].team, T[j].name) == 0) {
                insertPlayer(&T[j], newNode(&players[i]));
                break;
            }
        }
    }
}

void updateAvgSR(Team *t) {
    float sum = 0;
    int c = 0;
    PlayerNode *p = t->head;
    while (p) {
        if (strcmp(p->role, "Batsman") == 0 || strcmp(p->role, "All-rounder") == 0) {
            sum += p->strikeRate;
            c++;
        }
        p = p->next;
    }
    t->avgStrikeRate = c ? sum / c : 0;
}

void menuAddPlayer(Team T[]) {
    int id;
    PlayerNode *p = malloc(sizeof(PlayerNode));
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int idx = searchTeam(T, id);
    if (idx == -1) return;

    printf("Player ID: ");
    scanf("%d", &p->playerId);
    printf("Name: ");
    scanf("%s", p->name);
    printf("Role: ");
    scanf("%s", p->role);
    printf("Runs: ");
    scanf("%d", &p->totalRuns);
    printf("Avg: ");
    scanf("%f", &p->battingAverage);
    printf("SR: ");
    scanf("%f", &p->strikeRate);
    printf("Wickets: ");
    scanf("%d", &p->wickets);
    printf("Eco: ");
    scanf("%f", &p->economyRate);

    p->performanceIndex = computePI(p->role, p->battingAverage, p->strikeRate, p->wickets, p->economyRate);
    strcpy(p->teamName, T[idx].name);
    p->next = NULL;

    insertPlayer(&T[idx], p);
}

void menuShowTeam(Team *t) {
    PlayerNode *p = t->head;
    while (p) {
        printf("%d %s %s %.2f %.2f %.2f\n",
               p->playerId, p->name, p->role,
               p->strikeRate, p->battingAverage, p->performanceIndex);
        p = p->next;
    }
    updateAvgSR(t);
    printf("Players: %d AvgSR: %.2f\n", t->totalPlayers, t->avgStrikeRate);
}

void menuSortTeams(Team T[]) {
    for (int i = 0; i < teamCount; i++) updateAvgSR(&T[i]);
    for (int i = 0; i < teamCount - 1; i++)
        for (int j = i + 1; j < teamCount; j++)
            if (T[i].avgStrikeRate < T[j].avgStrikeRate) {
                Team x = T[i]; T[i] = T[j]; T[j] = x;
            }

    for (int i = 0; i < teamCount; i++)
        printf("%d %s %.2f %d\n", T[i].teamId, T[i].name, T[i].avgStrikeRate, T[i].totalPlayers);
}

void menuTopK(Team *t, const char *role, int K) {
    PlayerNode *a[300];
    int c = 0;
    PlayerNode *p = t->head;

    while (p) {
        if (strcmp(p->role, role) == 0)
            a[c++] = p;
        p = p->next;
    }

    for (int i = 0; i < c - 1; i++)
        for (int j = i + 1; j < c; j++)
            if (a[i]->performanceIndex < a[j]->performanceIndex) {
                PlayerNode *x = a[i]; a[i] = a[j]; a[j] = x;
            }

    for (int i = 0; i < K && i < c; i++)
        printf("%d %s %.2f\n", a[i]->playerId, a[i]->name, a[i]->performanceIndex);
}

void menuRoleAll(Team T[], const char *role) {
    PlayerNode *a[2000];
    int n = 0;

    for (int i = 0; i < teamCount; i++) {
        PlayerNode *p = T[i].head;
        while (p) {
            if (strcmp(p->role, role) == 0)
                a[n++] = p;
            p = p->next;
        }
    }

    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i]->performanceIndex < a[j]->performanceIndex) {
                PlayerNode *x = a[i]; a[i] = a[j]; a[j] = x;
            }

    for (int i = 0; i < n; i++)
        printf("%d %s %s %.2f\n", a[i]->playerId, a[i]->name, a[i]->teamName, a[i]->performanceIndex);
}

void runMenu(Team T[]) {
    while (1) {
        printf("1 Add\n2 Show Team\n3 Sort Teams\n4 Top K\n5 Show Role\n6 Exit\n");
        int ch;
        scanf("%d", &ch);
        if (ch == 6) return;

        if (ch == 1) menuAddPlayer(T);
        else if (ch == 2) {
            int id;
            scanf("%d", &id);
            int idx = searchTeam(T, id);
            if (idx != -1) menuShowTeam(&T[idx]);
        }
        else if (ch == 3) menuSortTeams(T);
        else if (ch == 4) {
            int id, K;
            char role[20];
            scanf("%d %s %d", &id, role, &K);
            int idx = searchTeam(T, id);
            if (idx != -1) menuTopK(&T[idx], role, K);
        }
        else if (ch == 5) {
            char role[20];
            scanf("%s", role);
            menuRoleAll(T, role);
        }
    }
}

int main() {
    Team T[10];
    initializeTeams(T);
    loadInitialPlayers(T);
    runMenu(T);
    return 0;
}
