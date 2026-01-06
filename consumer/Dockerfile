FROM gcc:12
WORKDIR /app
COPY consumer.c consumer.h ./
COPY common ./common
RUN gcc consumer.c -I./common -o consumer.out
CMD ["./consumer.out"]
