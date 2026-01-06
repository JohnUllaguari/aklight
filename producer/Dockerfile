FROM gcc:12
WORKDIR /app
COPY producer.c producer.h ./
COPY common ./common
RUN gcc producer.c -I./common -o producer.out
CMD ["./producer.out"]
