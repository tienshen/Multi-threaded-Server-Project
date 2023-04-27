**build server:**
    g++ -std=c++11 -pthread server.cpp -o server
**run server:**
./server -document_root <document_root_directory> -port <port_number>
**build client in another terminal:**
    g++ client.cpp -o client
**run client:**
    ./client <FILE>
