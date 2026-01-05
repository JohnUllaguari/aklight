FROM python:3.10-slim

WORKDIR /app

RUN pip install flask

COPY broker.py .

CMD ["python", "broker.py"]
