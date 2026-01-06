# 🧪 Guía de Pruebas - Proyecto AKLight

## ✅ Requisitos Implementados

### 1. Cluster de Brokers
- ✅ 2 brokers (broker1:5000, broker2:5001)
- ✅ Productores se conectan a brokers diferentes

### 2. Particiones
- ✅ 2 particiones por tópico
- ✅ Con clave → hash determina partición
- ✅ Sin clave → round-robin

### 3. Tópicos Multinivel
- ✅ `metrics/docker/cpu` (3 niveles)
- ✅ `metrics/docker/memory` (3 niveles)

### 4. Wildcard Multinivel (#)
- ✅ Consumidor suscrito a `metrics/docker/#`
- ✅ Recibe todos los mensajes bajo ese tópico

### 5. Persistencia de Mensajes
- ✅ Mensajes guardados en `partition_0.log` y `partition_1.log`
- ✅ Formato: `timestamp|topic|key|payload`

### 6. Sesiones Persistentes/No Persistentes
- ✅ Configurable con variable `PERSISTENT=1` o `PERSISTENT=0`
- ✅ Sesión persistente recibe mensajes históricos al reconectar

### 7. Métricas Múltiples
- ✅ Producer1 y Producer2 envían 2 métricas cada uno:
  - CPU (con clave)
  - Memoria (sin clave, round-robin)

---

## 🚀 Cómo Ejecutar

```bash
# Limpiar y construir
docker-compose down --remove-orphans
docker-compose build --no-cache
docker-compose up
```

---

## 📊 Qué Deberías Ver

### Broker1 (puerto 5000):
```
🚀 Broker escuchando en puerto 5000
👂 Consumidor registrado [consumer_1]: metrics/docker/# (persistente)
✉️ Productor conectado
📦 [P1] metrics/docker/cpu | usage: 67%
📦 [P0] metrics/docker/memory | usage: 345MB
```

### Broker2 (puerto 5001):
```
🚀 Broker escuchando en puerto 5000
✉️ Productor conectado
📦 [P1] metrics/docker/cpu | usage: 23%
📦 [P1] metrics/docker/memory | usage: 128MB
```

### Productor1 (conectado a broker1):
```
✅ Productor conectado a broker1
```

### Productor2 (conectado a broker2):
```
✅ Productor conectado a broker2
```

### Consumidor (conectado a broker1):
```
✅ Consumidor [consumer_1] conectado a broker1 - Suscrito a: metrics/docker/# (persistente)
📥 [Partición 1] [metrics/docker/cpu] usage: 67%
📥 [Partición 0] [metrics/docker/memory] usage: 345MB
📥 [Partición 1] [metrics/docker/cpu] usage: 23%
```

---

## 🧪 Pruebas Específicas

### Test 1: Particiones con Clave vs Round-Robin
**Observar**: Mensajes de CPU siempre van a la misma partición (tienen clave), mientras que memoria alterna entre P0 y P1.

### Test 2: Wildcard Multinivel
**Verificar**: Consumidor recibe tanto `metrics/docker/cpu` como `metrics/docker/memory` (wildcard `#`).

### Test 3: Persistencia de Mensajes
```bash
# Dentro del contenedor broker1
docker exec -it <broker1_container> cat partition_0.log
docker exec -it <broker1_container> cat partition_1.log
```

Deberías ver líneas como:
```
1704567890|metrics/docker/cpu|cpu-key|usage: 67%
1704567892|metrics/docker/memory||usage: 345MB
```

### Test 4: Sesión Persistente
1. Dejar correr por 10 segundos
2. Detener el consumidor: `docker-compose stop consumer`
3. Esperar 10 segundos más (broker sigue guardando mensajes)
4. Reiniciar consumidor: `docker-compose start consumer`
5. **Verificar**: Consumidor recibe los mensajes que perdió mientras estaba desconectado

### Test 5: Sesión No Persistente
Modificar docker-compose.yml:
```yaml
  consumer:
    environment:
      - PERSISTENT=0  # Cambiar a 0
```
Reiniciar y verificar que NO recibe mensajes históricos.

---

## 🐛 Si No Funciona

### Problema: No se ven mensajes
**Solución**: Los productores esperan 2 segundos antes de enviar. Espera al menos 5 segundos.

### Problema: Error de conexión
**Solución**: Verifica que los brokers estén arriba antes:
```bash
docker-compose logs broker1
docker-compose logs broker2
```

### Problema: Particiones siempre iguales
**Solución**: Normal. CPU tiene clave fija, va siempre a la misma partición. Memoria sin clave debería alternar.

---

## 📁 Archivos Importantes

- `protocol.h` - Protocolo de comunicación
- `broker.c` - Lógica del broker (particiones, wildcard, persistencia)
- `producer.c` - Envía 2 métricas (CPU con clave, memoria sin clave)
- `consumer.c` - Recibe con wildcard y sesión persistente
- `docker-compose.yml` - 2 brokers + 2 productores + 1 consumidor

---

## 🎯 Checklist Final

- [ ] 2 brokers funcionando (broker1, broker2)
- [ ] 2 productores conectados a brokers diferentes
- [ ] 1 consumidor con sesión persistente
- [ ] Tópicos de 3 niveles (`metrics/docker/cpu`)
- [ ] Wildcard `#` funcionando
- [ ] Particiones: CPU siempre misma, memoria alternando
- [ ] Archivos de persistencia creados (`partition_*.log`)
- [ ] Sesión persistente recibe mensajes históricos al reconectar