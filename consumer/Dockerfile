FROM python:3.10-slim

WORKDIR /app

RUN pip install requests

COPY consumer.py .

CMD ["python", "consumer.py"]
