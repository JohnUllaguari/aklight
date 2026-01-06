#ifndef BROKER_H
#define BROKER_H

#include "common/protocol.h"

#define MAX_PARTITIONS 4
#define MAX_MESSAGES_PER_PARTITION 100
#define MAX_TOPICS 10

// Estructura para una partición individual
typedef struct {
    message_t messages[MAX_MESSAGES_PER_PARTITION];
    int message_count;
    int write_index; // Índice circular para escribir
} partition_t;

// Estructura para un tópico con sus particiones
typedef struct {
    char topic_name[64];
    partition_t partitions[MAX_PARTITIONS];
    int partition_count;
    int next_partition; // Para round-robin
} topic_t;

// Gestión de tópicos
topic_t topics[MAX_TOPICS];
int topic_count = 0;

// Funciones principales
topic_t* find_or_create_topic(const char *topic_name, int partition_count);
int hash_key(const char *key);
void store_message(const char *topic_name, const char *key, message_t *msg);

#endif