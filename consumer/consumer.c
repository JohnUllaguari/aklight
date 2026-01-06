#include "consumer.h"
#include "common/protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    setbuf(stdout, NULL);

    int sock;
    struct sockaddr_in addr;
    handshake_t h;
    message_t msg;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROKER_PORT);
    inet_pton(AF_INET, "broker", &addr.sin_addr);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    h.role = ROLE_CONSUMER;
    send(sock, &h, sizeof(h), 0);

    printf("✅ Consumidor conectado\n");

    while (1) {
        if (recv(sock, &msg, sizeof(msg), 0) > 0) {
            printf("📥 [%s] %s\n", msg.topic, msg.payload);
        }
    }
}
