/*
 * server.c
 *
 *  Created on: 2025/10/16
 *      Author: daiki
 */

#include "ipc.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>

static void signal_handler(int sig) {
    printf("Signal %d received, stopping server...\n", sig);
    ipc_server_stop();
}

// データ受信時に呼ばれるコールバック
void on_receive(ipc_socket_t client_sock, const char *data, int len, void *user_data) {
    printf("Received from client: %s\n", data);

    // 受信データをそのまま返す（エコー）
    ipc_send(client_sock, data, len);
}

int main() {
    ipc_init();

    // Ctrl+Cなどで終了できるようにシグナルハンドラ登録
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    const char *ip = "0.0.0.0"; // 任意のインターフェースで待受
    int port = 12345;

    printf("Starting server on %s:%d...\n", ip, port);
    ipc_server_start(ip, port, on_receive, NULL);

    ipc_cleanup();
    printf("Server exited.\n");
    return 0;
}

