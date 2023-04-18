build server:
    g++ -std=c++11 -pthread server.cpp -o server
run server:
    ./server
build client in another terminal:
    g++ client.cpp -o client
run client:
    ./client