//
// Project: ECE670 multi-threaded HTTP server
// Authors: Tien Li Shen, Shivani
//

#include "server.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#define PORT 8096


using namespace std;

// Define a struct to hold connection information
struct Connection {
    int socket;
    sockaddr_in address;
};

// Define a queue to hold incoming connections
queue<Connection> connectionQueue;

// Define a mutex to ensure thread-safe access to the queue
mutex queueMutex;

//
std::string documentRoot;

void handleConnection(int socket_fd);


int main(int argc, char *argv[]) {

    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " -document_root <document_root_directory> -port <port_number>" << endl;
        return 1;
    }

    // Parse command line arguments
    int port;
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-document_root") == 0) {
            documentRoot = argv[i+1];
        } else if (strcmp(argv[i], "-port") == 0) {
            port = atoi(argv[i+1]);
        } else {
            cerr << "Invalid option: " << argv[i] << endl;
            return 1;
        }
    }

    int serverSocket, newSocket;
    sockaddr_in serverAddress;
    socklen_t serverAddressLen = sizeof(serverAddress);
    // Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }
    cout << "server starting...." << endl;
    // Bind the socket to an IP address and port
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);
    if (::bind(serverSocket, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Error binding socket" << endl;
        return 1;
    }
    cout << "server listening...." << endl;
    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        cerr << "Error listening for connections" << endl;
        return 1;
    }
    char server_add[50];
    gethostname(server_add, sizeof(server_add));
    cout << "Server address: " << server_add << ":" << ntohs(serverAddress.sin_port) << endl;

    while(true) {
        // Accept incoming connection
        if ((newSocket = accept(serverSocket, (struct sockaddr *) &serverAddress, (socklen_t *) &serverAddressLen)) <
            0) {
            cerr << "accept failed" << endl;
            return -1;
        }
        // spawn new thread to process the connection
        thread t(handleConnection, newSocket);
        t.detach();
    }

    return 0;

}

// Define a function to handle incoming connections
void handleConnection(int socket_fd) {
    cout << "Received a request from clients" << endl;
    char buffer[1024];
    int bytesReceived;
    string response;
    // Read data from the client
    bytesReceived = recv(socket_fd, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        cerr << "Error reading from socket" << endl;
        close(socket_fd);
        return;
    }

    // Parse the incoming request
    string request(buffer, bytesReceived);
    size_t requestEnd = request.find("\r\n\r\n");
    if (requestEnd == string::npos) {
        cerr << "Invalid request" << endl;
        close(socket_fd);
        return;
    }
    string requestHeader = request.substr(0, requestEnd);
    string requestBody = request.substr(requestEnd + 4);

    // Check if the request is a GET request
    if (requestHeader.find("GET ") == 0) {
        // Extract the file path from the request
        size_t pathStart = requestHeader.find("/") + 1;
        size_t pathEnd = requestHeader.find(" ", pathStart);
        if (pathStart == string::npos || pathEnd == string::npos) {
            cerr << "Invalid request" << endl;
            response = "400 Bad request";
            send(socket_fd, response.c_str(), response.length(), 0);
            close(socket_fd);
            return;
        }
        string filePath = requestHeader.substr(pathStart, pathEnd - pathStart);

        // If file path is not specified, serve a default index.html file
        if (filePath == "") {
            filePath = "index.html";
        }

        //new part
        // Check if the requested file is an image file
        bool isImage = false;
        if (filePath.find(".jpg") != string::npos || filePath.find(".jpeg") != string::npos || filePath.find(".png") != string::npos) {
            isImage = true;
        }

        // Open the requested file
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open()) {
            cerr << "Error opening file at filepath: " << filePath << endl;
            response = "404 Page not found";
            send(socket_fd, response.c_str(), response.length(), 0);
            close(socket_fd);
            return;
        }
        // Check for file permissions
        else if (errno == EACCES) {
            cerr << "File permission denied for filepath: " << filePath << endl;
            response = "403 Forbidden";
            send(socket_fd, response.c_str(), response.length(), 0);
            close(socket_fd);
            return;
        }

        // Read the contents of the file into a stringstream
        std::stringstream fileContent;
        fileContent << file.rdbuf();

        // Get the string from the stringstream
        string fileContents = fileContent.str();
        // Close the file
        file.close();

        // Send the response
        if (isImage) {
            response = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: " + to_string(fileContent.str().length()) + "\r\n\r\n";
            send(socket_fd, response.c_str(), response.length(), 0);
            send(socket_fd, fileContent.str().c_str(), fileContent.str().length(), 0);
            cout << "image response"  << response << endl;
        } else {
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + to_string(fileContents.length()) + "\r\n\r\n";
            send(socket_fd, response.c_str(), response.length(), 0);
            send(socket_fd, fileContents.c_str(), fileContents.length(), 0);
            cout << "html response"  << response << endl;
            // Open the file in the default web browser
            //string command = "open " + filePath;  // use "start" instead of "open" on Windows
            //system(command.c_str());
        }
    }

    // Close the connection
    close(socket_fd);
}

