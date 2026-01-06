#include "broker.h"
#include "../common/protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

void start_broker() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("🚀 Broker escuchando en puerto %d\n", PORT);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        memset(buffer, 0, BUFFER_SIZE);

        read(client_fd, buffer, BUFFER_SIZE);
        printf("📥 Mensaje recibido: %s", buffer);

        // Aquí luego separas PUBLISH / SUBSCRIBE
        write(client_fd, buffer, strlen(buffer));
        close(client_fd);
    }
}

int main() {
    start_broker();
    return 0;
}
