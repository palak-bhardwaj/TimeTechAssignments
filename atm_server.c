#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

pthread_mutex_t mutex;

void processClient(int client) {
    int choice, amount, balance;
    FILE *fp;

    read(client, &choice, sizeof(choice));

    pthread_mutex_lock(&mutex);
    fp = fopen("accountDB.txt", "r+");
    fscanf(fp, "%d", &balance);

    if (choice == 1) {
        read(client, &amount, sizeof(amount));
        if (amount <= balance) {
            balance -= amount;
            fseek(fp, 0, SEEK_SET);
            fprintf(fp, "%d", balance);
        } else balance = -1;
    } 
    else if (choice == 2) {
        read(client, &amount, sizeof(amount));
        balance += amount;
        fseek(fp, 0, SEEK_SET);
        fprintf(fp, "%d", balance);
    }

    fclose(fp);
    pthread_mutex_unlock(&mutex);

    write(client, &balance, sizeof(balance));
    close(client);
}

void* clientThread(void* arg) {
    processClient(*(int*)arg);
    return NULL;
}

void startServer() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    pthread_t tid;

    pthread_mutex_init(&mutex, NULL);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("ATM Server Running...\n");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        pthread_create(&tid, NULL, clientThread, &client_fd);
    }
}

int main() {
    startServer();
    return 0;
}
