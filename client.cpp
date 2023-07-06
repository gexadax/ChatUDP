#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#define MESSAGE_BUFFER 4096
#define PORT 7777

char buffer[MESSAGE_BUFFER];
char message[MESSAGE_BUFFER];
#ifdef _WIN32
SOCKET socket_descriptor;
#else
int socket_descriptor;
#endif
struct sockaddr_in serveraddress;

// Function to send a request to the server
void sendRequest() {
    // Set the server address
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddress.sin_port = htons(PORT);
    serveraddress.sin_family = AF_INET;

    // Create a socket
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
#else
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif

    // Establish a connection with the server
    if (connect(socket_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
#ifdef _WIN32
        closesocket(socket_descriptor);
        WSACleanup();
#else
        close(socket_descriptor);
#endif
        return;
    }

    while (true) {
        std::cout << "Enter a message to send to the server: " << std::endl;
        std::cin >> message;

        if (strcmp(message, "end") == 0) {
            // Send the termination message to the server
            sendto(socket_descriptor, message, static_cast<int>(strlen(message)), 0, nullptr, sizeof(serveraddress));
            std::cout << "Client work is done!" << std::endl;
#ifdef _WIN32
            closesocket(socket_descriptor);
            WSACleanup();
#else
            close(socket_descriptor);
#endif
            return;
        }
        else {
            // Send the message to the server
            sendto(socket_descriptor, message, static_cast<int>(strlen(message)), 0, nullptr, sizeof(serveraddress));
            std::cout << "Message sent successfully to the server: " << message << std::endl;
            std::cout << "Waiting for a response from the server..." << std::endl;
        }

        std::cout << "Message received from server: " << std::endl;
        // Receive the response from the server
        recvfrom(socket_descriptor, buffer, sizeof(buffer), 0, nullptr, nullptr);
        std::cout << buffer << std::endl;
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return 1;
    }
#endif

    std::cout << "CLIENT IS ESTABLISHING A CONNECTION WITH SERVER THROUGH PORT: " << PORT << std::endl;
    sendRequest();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
