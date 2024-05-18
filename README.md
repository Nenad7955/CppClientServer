Чтобы собрать проект под macos

g++ server.cpp -lpthread -std=c++17 -o server.out  
g++ client.cpp -lpthread -std=c++17 -o client.out 

Затем запускаем:
./server.out
./client.out