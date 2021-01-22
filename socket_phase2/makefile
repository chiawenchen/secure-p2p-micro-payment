all: client

client: client.o
	g++ client.o -o client
client.o: client.cpp
	g++ -c client.cpp

clean:
	rm client.o client
