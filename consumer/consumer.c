#include "consumer.h"
#include "common/protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

int main() {
    setbuf(stdout, NULL);

    int sock;
    struct sockaddr_in addr;
    handshake_t h;
    message_t msg;

    char *broker_host = getenv("BROKER_HOST");
    if (broker_host == NULL) broker_host = "broker";
    
    char *persistent_str = getenv("PERSISTENT");
    int persistent = (persistent_str != NULL && strcmp(persistent_str, "1") == 0);
    
    char *client_id = getenv("CLIENT_ID");
    if (client_id == NULL) client_id = "consumer_default";

    sock = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROKER_PORT);

    //resolver hostname Docker correctamente
    struct hostent *he = gethostbyname(broker_host);
    if (he == NULL) {
        perror("gethostbyname");
        exit(1);
    }
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    //verificar connect
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        exit(1);
    }

    memset(&h, 0, sizeof(h));
    h.role = ROLE_CONSUMER;
    strcpy(h.subscription, "metrics/docker/#");
    h.persistent = persistent;
    strcpy(h.client_id, client_id);

    send(sock, &h, sizeof(h), 0);

    printf("✅ Consumidor [%s] conectado a %s - Suscrito a: %s (%s)\n", 
           client_id, broker_host, h.subscription,
           persistent ? "persistente" : "no persistente");

    while (1) {
        memset(&msg, 0, sizeof(msg));

        int n = recv(sock, &msg, sizeof(msg), 0);
        if (n > 0) {
            printf("📥 [Partición %d] [%s] %s\n",
                   msg.partition, msg.topic, msg.payload);
        } 
        else if (n == 0) {
            printf("⚠️ Broker cerró la conexión\n");
            break;
        } 
        else {
            perror("recv");
            break;
        }
    }

    close(sock);
    return 0;
}
