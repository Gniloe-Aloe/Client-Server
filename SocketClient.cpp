#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

int main()
{
    WSADATA wsaDATA;
    ADDRINFO hints;
    ADDRINFO* addrResult = NULL;
    SOCKET ConnectSocket = INVALID_SOCKET;

    const char* sendBuffer = "Hello from client!";
    char recvBuffer[512];

    int result;

    result = WSAStartup(MAKEWORD(2, 2), &wsaDATA);
    if (result != 0) {
        std::cout << "WSAStartup failed, result = " << result << std::endl;
        return 1;
    }

    //зануляем память
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //принимет адрес или имя компьютера, порт и интерфейс
    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        std::cout << " getaddrinfo failed, result = " << result << std::endl;
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed" << std::endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        std::cout << "Unable connect to server" << std::endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    //тут соединились с сервером. отправляем
    //send возвращает количество переданных данных или SOCKET_ERROR
    result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
    if (result == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    std::cout << "Bytes sent: " << result << std::endl;;
    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        std::cout << "Shutdown error" << result << std::endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    
    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            std::cout << "Received bytes: " << result << std::endl;
            std::cout << "Received data: " << recvBuffer << std::endl;
        }
        else if (result == 0)
            std::cout << "Connection closed" << std::endl;
        else
            std::cout << "recv failed with error" << std::endl;
    } while (result > 0);

    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();

    return 0;
}


