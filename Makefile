all:
	gcc broker/broker.c -o broker.out
	gcc producer/producer.c -o producer.out
	gcc consumer/consumer.c -o consumer.out

clean:
	rm -f *.out
