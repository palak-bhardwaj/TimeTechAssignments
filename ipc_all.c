#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

void sortArray(int a[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (a[i] > a[j]) {
                int t = a[i];
                a[i] = a[j];
                a[j] = t;
            }
}

void fileIPC() {
    int n, a[50];
    FILE *fp;

    printf("Enter size: ");
    scanf("%d", &n);
    printf("Enter elements:\n");
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    fp = fopen("data.txt", "w");
    fprintf(fp, "%d\n", n);
    for (int i = 0; i < n; i++) fprintf(fp, "%d ", a[i]);
    fclose(fp);

    printf("Before Sorting: ");
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");

    if (fork() == 0) {
        fp = fopen("data.txt", "r");
        fscanf(fp, "%d", &n);
        for (int i = 0; i < n; i++) fscanf(fp, "%d", &a[i]);
        fclose(fp);

        sortArray(a, n);

        fp = fopen("data.txt", "w");
        fprintf(fp, "%d\n", n);
        for (int i = 0; i < n; i++) fprintf(fp, "%d ", a[i]);
        fclose(fp);
        exit(0);
    }

    wait(NULL);

    fp = fopen("data.txt", "r");
    fscanf(fp, "%d", &n);
    for (int i = 0; i < n; i++) fscanf(fp, "%d", &a[i]);
    fclose(fp);

    printf("After Sorting: ");
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");
}

// ---------------- PIPE IPC ----------------
void pipeIPC() {
    int fd[2], n, a[50];
    pipe(fd);

    printf("Enter size: ");
    scanf("%d", &n);
    printf("Enter elements:\n");
    for (int i = 0; i < n; i++) scanf("%d", &a[i]);

    printf("Before Sorting: ");
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");

    if (fork() == 0) {
        read(fd[0], &n, sizeof(n));
        read(fd[0], a, n * sizeof(int));
        sortArray(a, n);
        write(fd[1], a, n * sizeof(int));
        exit(0);
    }

    write(fd[1], &n, sizeof(n));
    write(fd[1], a, n * sizeof(int));
    wait(NULL);
    read(fd[0], a, n * sizeof(int));

    printf("After Sorting: ");
    for (int i = 0; i < n; i++) printf("%d ", a[i]);
    printf("\n");
}

struct msg {
    long type;
    int n;
    int arr[50];
};

void messageQueueIPC() {
    key_t key = ftok("msgfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg m;

    printf("Enter size: ");
    scanf("%d", &m.n);
    printf("Enter elements:\n");
    for (int i = 0; i < m.n; i++) scanf("%d", &m.arr[i]);

    printf("Before Sorting: ");
    for (int i = 0; i < m.n; i++) printf("%d ", m.arr[i]);
    printf("\n");

    m.type = 1;
    msgsnd(msgid, &m, sizeof(m), 0);

    if (fork() == 0) {
        msgrcv(msgid, &m, sizeof(m), 1, 0);
        sortArray(m.arr, m.n);
        m.type = 2;
        msgsnd(msgid, &m, sizeof(m), 0);
        exit(0);
    }

    msgrcv(msgid, &m, sizeof(m), 2, 0);
    printf("After Sorting: ");
    for (int i = 0; i < m.n; i++) printf("%d ", m.arr[i]);
    printf("\n");

    msgctl(msgid, IPC_RMID, NULL);
}

void sharedMemoryIPC() {
    int shmid = shmget(IPC_PRIVATE, 1024, 0666 | IPC_CREAT);
    int *shm = shmat(shmid, NULL, 0);

    int n;
    printf("Enter size: ");
    scanf("%d", &n);
    shm[0] = n;

    printf("Enter elements:\n");
    for (int i = 0; i < n; i++) scanf("%d", &shm[i + 1]);

    printf("Before Sorting: ");
    for (int i = 1; i <= n; i++) printf("%d ", shm[i]);
    printf("\n");

    if (fork() == 0) {
        sortArray(&shm[1], shm[0]);
        exit(0);
    }

    wait(NULL);

    printf("After Sorting: ");
    for (int i = 1; i <= n; i++) printf("%d ", shm[i]);
    printf("\n");

    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
}

int main() {
    int choice;
    printf("\n1.File IPC\n2.Pipe IPC\n3.Message Queue IPC\n4.Shared Memory IPC\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1) fileIPC();
    else if (choice == 2) pipeIPC();
    else if (choice == 3) messageQueueIPC();
    else if (choice == 4) sharedMemoryIPC();

    return 0;
}
