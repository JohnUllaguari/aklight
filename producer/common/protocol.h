#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BROKER_PORT 5000

#define ROLE_PRODUCER 1
#define ROLE_CONSUMER 2

typedef struct {
    int role;
} handshake_t;

typedef struct {
    char topic[64];
    char payload[256];
} message_t;

#endif
