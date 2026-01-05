import requests
import os
import time

BROKER_HOST = os.getenv("BROKER_HOST", "broker1")
BROKER_PORT = 5000

TOPIC = os.getenv("TOPIC", "metrics/docker/#")
PERSISTENT = os.getenv("PERSISTENT", "false").lower() == "true"

OFFSET_FILE = "consumer_offset.txt"

def get_messages():
    url = f"http://{BROKER_HOST}:{BROKER_PORT}/consume"
    params = {"topic": TOPIC}
    response = requests.get(url, params=params)
    return response.json().get("messages", [])

def load_offset():
    if PERSISTENT and os.path.exists(OFFSET_FILE):
        with open(OFFSET_FILE) as f:
            return int(f.read())
    return 0

def save_offset(offset):
    if PERSISTENT:
        with open(OFFSET_FILE, "w") as f:
            f.write(str(offset))

offset = load_offset()

while True:
    messages = get_messages()

    for i in range(offset, len(messages)):
        print(messages[i].strip())

    offset = len(messages)
    save_offset(offset)

    time.sleep(5)
