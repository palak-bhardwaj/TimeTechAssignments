#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

void startClient() {
    int sock, choice, amount, balance;
    struct sockaddr_in addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    printf("1.Withdraw\n2.Deposit\n3.Display Balance\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    write(sock, &choice, sizeof(choice));

    if (choice == 1 || choice == 2) {
        printf("Enter amount: ");
        scanf("%d", &amount);
        write(sock, &amount, sizeof(amount));
    }

    read(sock, &balance, sizeof(balance));

    if (balance == -1)
        printf("Transaction Failed\n");
    else
        printf("Current Balance: %d\n", balance);

    close(sock);
}

int main() {
    startClient();
    return 0;
}
