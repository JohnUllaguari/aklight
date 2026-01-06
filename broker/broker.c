#include "broker.h"
#include "common/protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>

#define MAX_CLIENTS 10

int consumers[MAX_CLIENTS];
int consumer_count = 0;

int main() {
    setbuf(stdout, NULL);

    int server_fd, client_fd, max_fd;
    fd_set readfds;
    struct sockaddr_in addr;
    handshake_t h;
    message_t msg;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROKER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    printf("🚀 Broker escuchando en puerto %d\n", BROKER_PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_fd = server_fd;

        for (int i = 0; i < consumer_count; i++) {
            FD_SET(consumers[i], &readfds);
            if (consumers[i] > max_fd) max_fd = consumers[i];
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(server_fd, &readfds)) {
            client_fd = accept(server_fd, NULL, NULL);

            recv(client_fd, &h, sizeof(h), 0);

            if (h.role == ROLE_CONSUMER) {
                consumers[consumer_count++] = client_fd;
                printf("👂 Consumidor registrado (%d)\n", consumer_count);
            }

            if (h.role == ROLE_PRODUCER) {
                printf("✉️ Productor conectado\n");

                while (recv(client_fd, &msg, sizeof(msg), 0) > 0) {
                    printf("📦 %s | %s\n", msg.topic, msg.payload);
                    for (int i = 0; i < consumer_count; i++) {
                        send(consumers[i], &msg, sizeof(msg), 0);
                    }
                }
                close(client_fd);
            }
        }
    }
}
