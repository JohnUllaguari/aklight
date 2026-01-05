from flask import Flask, request, jsonify
import os
import time
import hashlib

app = Flask(__name__)

BROKER_ID = os.getenv("BROKER_ID", "1")
DATA_PATH = "/data"
PARTITIONS = 2

# Round-robin counters por tópico
round_robin_index = {}

# -------------------------------------------------
# Utilidades
# -------------------------------------------------

def ensure_path(topic):
    path = os.path.join(DATA_PATH, *topic.split("/"))
    os.makedirs(path, exist_ok=True)
    return path

def select_partition(topic, key):
    if key:
        return int(hashlib.md5(key.encode()).hexdigest(), 16) % PARTITIONS
    else:
        if topic not in round_robin_index:
            round_robin_index[topic] = 0
        partition = round_robin_index[topic]
        round_robin_index[topic] = (partition + 1) % PARTITIONS
        return partition

# -------------------------------------------------
# Endpoint: Publicar mensaje (Productor)
# -------------------------------------------------

@app.route("/publish", methods=["POST"])
def publish():
    data = request.json
    topic = data.get("topic")
    key = data.get("key")
    value = data.get("value")

    if not topic or not value:
        return jsonify({"error": "topic y value requeridos"}), 400

    partition = select_partition(topic, key)
    topic_path = ensure_path(topic)
    file_path = os.path.join(topic_path, f"partition{partition}.log")

    with open(file_path, "a") as f:
        f.write(f"{int(time.time())}|{key}|{value}\n")

    return jsonify({
        "broker": BROKER_ID,
        "topic": topic,
        "partition": partition,
        "status": "stored"
    })

# -------------------------------------------------
# Endpoint: Consumir mensajes (Consumidor)
# -------------------------------------------------

@app.route("/consume", methods=["GET"])
def consume():
    topic = request.args.get("topic")

    if not topic:
        return jsonify({"error": "topic requerido"}), 400

    results = []

    for root, dirs, files in os.walk(DATA_PATH):
        relative = root.replace(DATA_PATH + "/", "")
        if match_topic(topic, relative):
            for file in files:
                with open(os.path.join(root, file)) as f:
                    results.extend(f.readlines())

    return jsonify({"messages": results})

# -------------------------------------------------
# Wildcard multi-nivel (#)
# -------------------------------------------------

def match_topic(subscription, topic):
    if subscription.endswith("/#"):
        base = subscription[:-2]
        return topic.startswith(base)
    return subscription == topic

# -------------------------------------------------

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
