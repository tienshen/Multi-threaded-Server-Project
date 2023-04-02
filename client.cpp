//
// Created by Tien Shen on 3/31/23.
//

#include "client.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <cstring>
#include <netdb.h>

#define HOST "elnux1.cs.umass.edu"
#define PORT "8095"

using namespace std;

// Define a function to request a file from the server
string getFile(string host, string port, string filePath) {
    int socketId = socket(AF_INET, SOCK_STREAM, 0);
    if (socketId < 0) {
        cerr << "Error creating socket" << endl;
        return "";
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Attempt to connect to server
    int status = getaddrinfo(HOST, PORT, &hints, &res);
    if (status != 0) {
        std::cerr << "Error getting address info: " << gai_strerror(status) << std::endl;
        return "";
    }

    if (connect(socketId, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        return "";
    }

    string request = "GET " + filePath + " HTTP/1.1\r\n\r\n";
    if (send(socketId, request.c_str(), request.length(), 0) < 0) {
        cerr << "Error sending request" << endl;
        close(socketId);
        return "";
    }

    string response;
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(socketId, buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, bytesReceived);
    }

    close(socketId);
    return response;
}

int main(){
    string response = getFile(HOST, PORT, "/example.txt");
    cout << response << endl;
}