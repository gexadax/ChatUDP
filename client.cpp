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

void sendRequest() {
    // Укажем адрес сервера
    serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Зададим номер порта для соединения с сервером
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;

    // Создание сокета
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << std::endl << "Failed to initialize winsock" << std::endl;
        return;
    }
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
#else
    socket_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif

    // Установка соединения с сервером
    if (connect(socket_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress)) < 0) {
        std::cout << std::endl << "Something went wrong. Connection Failed." << std::endl;
#ifdef _WIN32
        closesocket(socket_descriptor);
        WSACleanup();
#else
        close(socket_descriptor);
#endif
        return;
    }

    while (1) {
        std::cout << "Enter a message you want to send to the server: " << std::endl;
        std::cin >> message;
        if (strcmp(message, "end") == 0) {
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
            sendto(socket_descriptor, message, static_cast<int>(strlen(message)), 0, nullptr, sizeof(serveraddress));
            std::cout << "Message sent successfully to the server: " << message << std::endl;
            std::cout << "Waiting for the Response from Server..." << std::endl;
        }

        std::cout << "Message Received From Server: " << std::endl;
        recvfrom(socket_descriptor, buffer, sizeof(buffer), 0, nullptr, nullptr);
        std::cout << buffer << std::endl;
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed to initialize winsock" << std::endl;
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
