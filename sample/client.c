/*
 * client.c
 *
 *  Created on: 2025/10/16
 *      Author: daiki
 */

#include "ipc.h"
#include <stdio.h>
#include <string.h>

int main() {
    ipc_init();

    const char *ip = "127.0.0.1"; // ローカル接続
    int port = 12345;

    ipc_socket_t sock = ipc_client_connect(ip, port);
    if (sock == INVALID_SOCKET) {
        ipc_cleanup();
        return 1;
    }

    char input[256];
    while (1) {
        printf("Input message (or 'quit'): ");
        if (!fgets(input, sizeof(input), stdin)) break;

        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') input[len-1] = '\0';

        if (strcmp(input, "quit") == 0) break;

        ipc_send(sock, input, strlen(input));

        char buffer[1024];
        int rlen = ipc_recv(sock, buffer, sizeof(buffer)-1);
        if (rlen > 0) {
            buffer[rlen] = '\0';
            printf("Received echo: %s\n", buffer);
        }
    }

    ipc_close(sock);
    ipc_cleanup();
    return 0;
}

