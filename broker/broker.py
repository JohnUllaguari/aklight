import socket
import threading
import json
import os
import time
import hashlib

HOST = "0.0.0.0"
PORT = 5000
DATA_PATH = "/data"
PARTITIONS = 2

round_robin = {}

def ensure_path(topic):
    path = os.path.join(DATA_PATH, *topic.split("/"))
    os.makedirs(path, exist_ok=True)
    return path

def select_partition(topic, key):
    if key:
        return int(hashlib.md5(key.encode()).hexdigest(), 16) % PARTITIONS
    round_robin[topic] = (round_robin.get(topic, -1) + 1) % PARTITIONS
    return round_robin[topic]

def match_topic(sub, topic):
    if sub.endswith("/#"):
        return topic.startswith(sub[:-2])
    return sub == topic

def handle_client(conn):
    print("📥 Cliente conectado", flush=True)
    buffer = ""
    with conn:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            buffer += data.decode()

            while "\n" in buffer:
                line, buffer = buffer.split("\n", 1)
                msg = json.loads(line)

                if msg["type"] == "publish":
                    topic = msg["topic"]
                    key = msg.get("key")
                    value = msg["value"]

                    part = select_partition(topic, key)
                    path = ensure_path(topic)
                    file = os.path.join(path, f"partition{part}.log")

                    with open(file, "a") as f:
                        producer = msg.get("key", "unknown")
                        f.write(f"{int(time.time())}|{producer}|{value}\n")


                    print(f"📦 Guardado {topic} → partition {part}", flush=True)
                    conn.sendall(b'{"status":"stored"}\n')

                elif msg["type"] == "consume":
                    sub = msg["topic"]
                    messages = []

                    for root, _, files in os.walk(DATA_PATH):
                        rel = root.replace(DATA_PATH + "/", "")
                        if match_topic(sub, rel):
                            for f in files:
                                with open(os.path.join(root, f)) as fd:
                                    messages.extend(fd.readlines())

                    conn.sendall((json.dumps(messages) + "\n").encode())

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((HOST, PORT))
    s.listen()
    print(f"🚀 Broker escuchando en puerto {PORT}", flush=True)

    while True:
        conn, _ = s.accept()
        threading.Thread(target=handle_client, args=(conn,), daemon=True).start()

if __name__ == "__main__":
    main()
