import requests
import time
import psutil
import os

BROKER_HOST = os.getenv("BROKER_HOST", "broker1")
BROKER_PORT = 5000
INTERVAL = 5  # segundos

PRODUCER_ID = os.getenv("HOSTNAME", "producer")

def send_metric(topic, value):
    url = f"http://{BROKER_HOST}:{BROKER_PORT}/publish"
    payload = {
        "topic": topic,
        "key": PRODUCER_ID,
        "value": value
    }
    try:
        requests.post(url, json=payload)
    except Exception as e:
        print("Error enviando métrica:", e)

while True:
    cpu = psutil.cpu_percent()
    mem = psutil.virtual_memory().percent

    send_metric("metrics/docker/cpu", f"{cpu}%")
    send_metric("metrics/docker/memory", f"{mem}%")

    time.sleep(INTERVAL)
