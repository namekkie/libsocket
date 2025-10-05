#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib") // Winsockライブラリリンク

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // サーバーと同じポート
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // ローカルホスト

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connect failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Connected to server!\n");

    // サーバーからのメッセージ受信
    char buffer[256];
    int len = recv(sock, buffer, sizeof(buffer)-1, 0);
    if (len > 0) {
        buffer[len] = '\0'; // 文字列化
        printf("Received from server: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
