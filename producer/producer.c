#include "producer.h"
#include "common/protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <netdb.h>

int main() {
    setbuf(stdout, NULL);
    srand(time(NULL) + getpid());

    int sock;
    struct sockaddr_in addr;
    handshake_t h;
    message_t msg;

    char *broker_host = getenv("BROKER_HOST");
    if (broker_host == NULL) broker_host = "broker";

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROKER_PORT);

    struct hostent *he = gethostbyname(broker_host);
    if (he == NULL) {
        perror("gethostbyname");
        exit(1);
    }
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    h.role = ROLE_PRODUCER;
    h.subscription[0] = '\0';
    h.persistent = 0;
    h.client_id[0] = '\0';

    send(sock, &h, sizeof(h), 0);

    printf("✅ Productor conectado a %s\n", broker_host);

    sleep(3);

    int counter = 0;
    while (1) {
        memset(&msg, 0, sizeof(msg));

        if (counter % 2 == 0) {
            strcpy(msg.topic, "metrics/docker/cpu");
            snprintf(msg.payload, sizeof(msg.payload),
                     "usage: %d%%", rand() % 100);
            strcpy(msg.key, "cpu-key");
        } else {
            strcpy(msg.topic, "metrics/docker/memory");
            snprintf(msg.payload, sizeof(msg.payload),
                     "usage: %dMB", 100 + rand() % 900);
        }

        send(sock, &msg, sizeof(msg), 0);
        counter++;
        sleep(2);
    }
}
