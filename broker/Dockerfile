FROM gcc:12
WORKDIR /app
COPY broker.c broker.h ./
COPY common ./common
RUN gcc broker.c -I./common -o broker.out
CMD ["./broker.out"]
