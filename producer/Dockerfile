FROM python:3.10-slim
WORKDIR /app
RUN pip install psutil
COPY producer.py .
CMD ["python", "producer.py"]
