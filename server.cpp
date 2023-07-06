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
int socket_file_descriptor;

#ifdef _WIN32
typedef int socklen_t;
#endif

socklen_t message_size, length;
const char* end_string = "end";
struct sockaddr_in serveraddress, client;

void processRequest() {
    // Initialize Winsock on Windows
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }
#endif

    // Create UDP socket
    socket_file_descriptor = static_cast<int>(socket(AF_INET, SOCK_DGRAM, 0));
    if (socket_file_descriptor == -1) {
        std::cerr << "Failed to create socket" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return;
    }

    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddress.sin_port = htons(PORT);
    serveraddress.sin_family = AF_INET;

    // Bind socket to address and port
    if (bind(socket_file_descriptor, reinterpret_cast<struct sockaddr*>(&serveraddress),
        sizeof(serveraddress)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
#ifdef _WIN32
        closesocket(socket_file_descriptor);
        WSACleanup();
#else
        close(socket_file_descriptor);
#endif
        return;
    }

    while (true) {
        length = sizeof(client);

        // Receive data from client
        message_size = recvfrom(socket_file_descriptor, buffer, sizeof(buffer) - 1, 0,
            reinterpret_cast<struct sockaddr*>(&client), &length);
        if (message_size == -1) {
            std::cerr << "Failed to receive data from client" << std::endl;
#ifdef _WIN32
            closesocket(socket_file_descriptor);
            WSACleanup();
#else
            close(socket_file_descriptor);
#endif
            return;
        }
        buffer[message_size] = '\0';

        // Check for termination signal
        if (strcmp(buffer, end_string) == 0) {
            std::cout << "Server is quitting" << std::endl;
#ifdef _WIN32
            closesocket(socket_file_descriptor);
            WSACleanup();
#else
            close(socket_file_descriptor);
#endif
            return;
        }

        std::cout << "Message Received from Client: " << buffer << std::endl;

        // Reply to the client
        std::cout << "Enter reply message to the client: " << std::endl;
        std::cin >> message;
        sendto(socket_file_descriptor, message, strlen(message), 0,
            reinterpret_cast<struct sockaddr*>(&client), sizeof(client));
        std::cout << "Message Sent Successfully to the client: " << message << std::endl;
        std::cout << "Waiting for the Reply from Client.." << std::endl;
    }

    // Clean up resources
#ifdef _WIN32
    closesocket(socket_file_descriptor);
    WSACleanup();
#else
    close(socket_file_descriptor);
#endif
}

int main() {
    std::cout << "SERVER IS LISTENING THROUGH THE PORT: " << PORT << " WITHIN A LOCAL SYSTEM" << std::endl;
    processRequest();
    return 0;
}
