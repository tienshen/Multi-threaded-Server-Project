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
#include <fstream>
#include <cstdlib> // for system()
#include <iostream>
#include <string>
//#define HOST "elnux1.cs.umass.edu"
#define HOST "localhost"
#define PORT "8096"

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
    int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "Error getting address info: " << gai_strerror(status) << std::endl;
        return "";
    }

    if (connect(socketId, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "Error connecting to server" << std::endl;
        return "";
    }

    /*
    string request = "GET " + filePath + " HTTP/1.1\r\n";
    request += "Host: " + host + ":" + port + "\r\n";
    request += "Connection: close\r\n\r\n";
    */

    string request, extension;
    size_t dotIndex = filePath.rfind('.');
    if (dotIndex != string::npos) {
        extension = filePath.substr(dotIndex + 1);
    }
    if (extension == "jpg" || extension == "jpeg") {
        request = "GET " + filePath + " HTTP/1.1\r\n";
        request += "Host: " + host + ":" + port + "\r\n";
        request += "Connection: close\r\n";
        request += "Accept: image/jpeg\r\n\r\n";
    } else if (extension == "html") {
        request = "GET " + filePath + " HTTP/1.1\r\n";
        request += "Host: " + host + ":" + port + "\r\n";
        request += "Connection: close\r\n\r\n";
    } else {
        cerr << "Unsupported file type" << endl;
        return "";
    }

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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <file_path>" << endl;
        return 1;
    }

    string filePath = argv[1];
    string response = getFile(HOST, PORT, filePath);

    if (response.find("Content-Type: image/jpeg") != string::npos ||
        response.find("Content-Type: image/png") != string::npos) {
        // Save image file
        ofstream file("example1.jpeg", ios::out | ios::binary);
        file << response;
        file.close();

        // Generate HTML file with image tag
        ofstream html(filePath + ".html");
        //html << "<html><body><img src=\"" << filePath << ".jpeg\"></body></html>";
        html << "<html><body><img src=\"<filePath.jpeg\"></body></html>";
        html.close();

        // Open image in browser
        system("xdg-open /home/group5/Desktop/Thread_Server_PNG/example1.jpeg"); // Replace with appropriate command for your OS
    } else {
        // Save HTML file
        ofstream file("example.html");
        file << response;
        file.close();

        // Open HTML in browser
        system("xdg-open /home/group5/Desktop/Thread_Server_PNG/example.html"); // Replace with appropriate command for your OS
    }


    /*

	if (response.find("Content-Type: image/jpeg") != string::npos ||
	    response.find("Content-Type: image/png") != string::npos) {
	    // Save image file
	    ofstream file("filePath.jpeg", ios::out | ios::binary);
            //ofstream file(filePath + ".jpeg", ios::out | ios::binary);
	    file << response;
	    file.close();

	    // Open image in browser
	    //ofstream html("<filePath>.html");
            ofstream html(filePath + ".html");
	    html << "<html><body><img src=\"<filePath.jpeg\"></body></html>";
            //html << "<html><body><img src=\"" << filePath << ".jpeg\"></body></html>";

	    html.close();
	    system("xdg-open filePath.html"); // Replace with appropriate command for your OS
            // system(("xdg-open " + filePath + ".html").c_str());
	} else {
	    // Save HTML file
	    ofstream file("filePath.html");
            //ofstream file(filePath);
	    file << response;
	    file.close();
	    system("xdg-open filePath.html"); // Replace with appropriate command for your OS
            //system(("xdg-open /home/group5/Desktop/Thread_Server_PNG" + filePath).c_str());
	} */

    /*if (response.find("Content-Type: image/jpeg") != string::npos ||
        response.find("Content-Type: image/png") != string::npos) {
        // Save image file
        ofstream file("example.jpeg", ios::out | ios::binary);
        file << response;
        file.close();
        system("xdg-open /home/group5/Desktop/Thread_Server_PNG/example.jpeg"); // Replace with appropriate command for your OS
    } else {
        // Save HTML file
        ofstream file("example.html");
        file << response;
        file.close();
        system("xdg-open /home/group5/Desktop/Thread_Server_PNG/example.html"); // Replace with appropriate command for your OS
    }*/

    return 0;
}






