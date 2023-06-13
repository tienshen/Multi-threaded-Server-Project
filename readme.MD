### Server Configuration:

In Server/Trial.html file, change the IP address and port number of the server.  
**Build Server:** ``g++ -std=c++11 -pthread server.cpp -o server``  
**Run Server:** ``./server``

### Client Configuration:

In the Client/client.cpp, change the IP address and port number of the server.   
**Build Client (In another terminal):** ``g++ client.cpp -o client``  
**Run Client:** ``./client /<Requested File name>`` 

Change the IP address and port number of the server in eval.py while load testing the server.
