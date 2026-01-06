#include "broker.h"
#include "common/protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string.h>
#include <time.h>

#define MAX_CLIENTS 10
#define NUM_PARTITIONS 2

typedef struct {
    int fd;
    int role;                     // producer / consumer
    char subscription[64];        // solo consumidores
    int persistent;
    char client_id[32];
    long last_timestamp;
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

int round_robin_counter = 0;

/* ================= UTILIDADES ================= */

int get_partition_by_key(char *key) {
    int sum = 0;
    for (int i = 0; key[i]; i++) sum += key[i];
    return sum % NUM_PARTITIONS;
}

int topic_matches(const char *topic, const char *sub) {
    int i = 0;
    while (topic[i] && sub[i]) {
        if (sub[i] == '#') return 1;
        if (topic[i] != sub[i]) return 0;
        i++;
    }
    if (sub[i] == '#') return 1;
    return topic[i] == '\0' && sub[i] == '\0';
}

void persist_message(message_t *msg) {
    char filename[64];
    snprintf(filename, sizeof(filename), "partition_%d.log", msg->partition);

    FILE *f = fopen(filename, "a");
    if (!f) return;

    fprintf(f, "%ld|%s|%s|%s\n",
            time(NULL), msg->topic, msg->key, msg->payload);
    fclose(f);
}

/* ================= MAIN ================= */

int main() {
    setbuf(stdout, NULL);

    int server_fd, max_fd;
    struct sockaddr_in addr;
    fd_set readfds;

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

        for (int i = 0; i < client_count; i++) {
            FD_SET(clients[i].fd, &readfds);
            if (clients[i].fd > max_fd)
                max_fd = clients[i].fd;
        }

        select(max_fd + 1, &readfds, NULL, NULL, NULL);

        /* ===== NUEVAS CONEXIONES ===== */
        if (FD_ISSET(server_fd, &readfds)) {
            int client_fd = accept(server_fd, NULL, NULL);

            handshake_t h;
            if (recv(client_fd, &h, sizeof(h), 0) <= 0) {
                close(client_fd);
                continue;
            }

            clients[client_count].fd = client_fd;
            clients[client_count].role = h.role;

            if (h.role == ROLE_CONSUMER) {
                strcpy(clients[client_count].subscription, h.subscription);
                clients[client_count].persistent = h.persistent;
                strcpy(clients[client_count].client_id, h.client_id);
                clients[client_count].last_timestamp = 0;

                printf("👂 Consumidor [%s] suscrito a %s\n",
                       h.client_id, h.subscription);
            } else {
                printf("✉️ Productor conectado\n");
            }

            client_count++;
        }

        /* ===== CLIENTES EXISTENTES ===== */
        for (int i = 0; i < client_count; i++) {
            if (!FD_ISSET(clients[i].fd, &readfds)) continue;

            message_t msg;
            int n = recv(clients[i].fd, &msg, sizeof(msg), 0);

            if (n <= 0) {
                close(clients[i].fd);
                clients[i] = clients[--client_count];
                i--;
                continue;
            }

            if (clients[i].role == ROLE_PRODUCER) {
                if (strlen(msg.key) > 0)
                    msg.partition = get_partition_by_key(msg.key);
                else
                    msg.partition = round_robin_counter++ % NUM_PARTITIONS;

                printf("📦 [P%d] %s → %s\n",
                       msg.partition, msg.topic, msg.payload);

                persist_message(&msg);

                for (int j = 0; j < client_count; j++) {
                    if (clients[j].role == ROLE_CONSUMER &&
                        topic_matches(msg.topic, clients[j].subscription)) {
                        send(clients[j].fd, &msg, sizeof(msg), 0);
                    }
                }
            }
        }
    }
}
