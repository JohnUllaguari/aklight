import socket
import json
import time
import os

BROKER_HOST = os.getenv("BROKER_HOST", "broker1")
PORT = 5000
TOPIC = os.getenv("TOPIC", "metrics/docker/#")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((BROKER_HOST, PORT))
print("✅ Consumidor conectado", flush=True)

while True:
    req = {
        "type": "consume",
        "topic": TOPIC
    }
    s.sendall((json.dumps(req) + "\n").encode())

    data = s.recv(8192)
    messages = json.loads(data.decode())

    print("📥 Mensajes recibidos:", flush=True)
    for m in messages[-5:]:
        print("   ", m.strip(), flush=True)

    time.sleep(5)
