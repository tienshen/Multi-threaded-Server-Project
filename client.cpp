//
// Created by Tien Shen on 3/31/23.
//

#include "client.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

using namespace std;

// Define a function to request a file from the server
string getFile(string host, int port, string filePath) {
    int socketId = socket(AF_INET, SOCK_STREAM, 0);
    if (socketId < 0) {
        cerr << "Error creating socket" << endl;
        return "";
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
        cerr << "Invalid server address" << endl;
        close(socketId);
        return "";
    }

    if (connect(socketId, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error connecting to server" << endl;
        close(socketId);
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
    string response = getFile("localhost", 8080, "/example.txt");
    cout << response << endl;
}