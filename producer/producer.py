import socket
import json
import time
import psutil
import os

BROKER_HOST = os.getenv("BROKER_HOST", "broker1")
PORT = 5000
PRODUCER_ID = os.getenv("HOSTNAME", "producer")

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

while True:
    try:
        s.connect((BROKER_HOST, PORT))
        print("✅ Productor conectado al broker", flush=True)
        break
    except:
        print("⏳ Esperando broker...", flush=True)
        time.sleep(2)

def send(topic, value):
    msg = {
        "type": "publish",
        "topic": topic,
        "key": PRODUCER_ID,
        "value": value
    }
    s.sendall((json.dumps(msg) + "\n").encode())
    print(f"📤 Enviado {topic}: {value}", flush=True)

while True:
    send("metrics/docker/cpu", f"{psutil.cpu_percent()}%")
    send("metrics/docker/memory", f"{psutil.virtual_memory().percent}%")
    time.sleep(5)
