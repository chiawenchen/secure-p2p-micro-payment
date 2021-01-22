all: server

.PHONY: server

server: server.o user_userList_class.o clientClass.o threadPoolClass.o
	g++ server.o user_userList_class.o clientClass.o threadPoolClass.o -o server -pthread

server.o: server.cpp const.h
	g++ -c server.cpp

user_userList_class.o: const.h user_userList_class.h user_userList_class.cpp
	g++ -c user_userList_class.cpp

clientClass.o: const.h clientClass.h clientClass.cpp
	g++ -c clientClass.cpp

threadPoolClass.o: const.h threadPoolClass.h threadPoolClass.cpp
	g++ -c threadPoolClass.cpp
clean:
	rm *.o