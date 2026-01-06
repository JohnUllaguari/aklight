#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BROKER_PORT 5000

#define ROLE_PRODUCER 1
#define ROLE_CONSUMER 2

typedef struct {
    int role;
    char subscription[64]; // Para consumidores: topic o pattern con #
    int persistent;        // 1 = sesión persistente, 0 = no persistente
    char client_id[32];    // ID único del cliente para sesión persistente
} handshake_t;

typedef struct {
    char topic[64];
    char payload[256];
    char key[32];  // Nueva: para determinar partición
    int partition; // Nueva: partición asignada
} message_t;

#endif