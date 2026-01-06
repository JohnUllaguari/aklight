#include "consumer.h"
#include "../common/protocol.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    write(sock, "SUBSCRIBE metrics/docker/#\n", 27);

    read(sock, buffer, BUFFER_SIZE);
    printf("📥 Recibido: %s\n", buffer);

    close(sock);
    return 0;
}
