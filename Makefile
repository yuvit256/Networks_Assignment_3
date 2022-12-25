.PHONY: all clean

all: Receiver Sender

Receiver: Receiver.o
	gcc -Wall -g -o Receiver Receiver.o

Sender: Sender.o
	gcc -Wall -g -o Sender Sender.o

Receiver.o: Receiver.c
	gcc -Wall -g -c Receiver.c

Sender.o: Sender.c
	gcc -Wall -g -c Sender.c

clean:
	rm -f *.o Receiver Sender

