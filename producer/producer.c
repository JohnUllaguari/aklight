#include "producer.h"
#include "common/protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>

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

    h.role = ROLE_PRODUCER;
    send(sock, &h, sizeof(h), 0);

    while (1) {
        snprintf(msg.topic, sizeof(msg.topic), "metrics/docker/cpu");
        snprintf(msg.payload, sizeof(msg.payload), "%d%%", rand() % 100);
        send(sock, &msg, sizeof(msg), 0);
        sleep(2);
    }
}
