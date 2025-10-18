/*
 * ipc.c
 *
 *  Created on: 2025/10/16
 *      Author: daiki
 */
#include "ipc.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
DWORD WINAPI client_thread(LPVOID param);
#else
void *client_thread(void *param);
#endif

typedef struct {
    ipc_socket_t client_sock;
    ipc_receive_callback callback;
    void *user_data;
} client_thread_arg_t;

// サーバー終了フラグ
static volatile int ipc_server_running = 0;

/**
 * @brief IPCライブラリの初期化を行う
 *
 * Windows環境ではソケット通信を行うために Winsock の初期化が必要。
 * Linuxでは不要なので、何もせず0を返す。
 *
 * @return 0: 成功, -1: 失敗
 */
int ipc_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    // Winsockライブラリを初期化
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return -1;
    }
#endif
    return 0;
}

/**
 * @brief IPCライブラリのクリーンアップを行う
 *
 * WindowsではWSACleanup()を呼び出してWinsockを終了。
 * Linuxでは何もしない。
 */
void ipc_cleanup(void) {
#ifdef _WIN32
    WSACleanup();
#endif
}

/**
 * @brief クライアント接続ごとに起動されるスレッド関数
 *
 * @param param client_thread_arg_tへのポインタ
 * @note Windows と Linuxで戻り値が異なる
 * @return DWORD スレッド終了コード
 */
#ifdef _WIN32
DWORD WINAPI client_thread(LPVOID param)
#else
void *client_thread(void *param)
#endif
{
    client_thread_arg_t *arg = (client_thread_arg_t *)param;
    ipc_socket_t sock = arg->client_sock;
    char buffer[1024];
    int len;

    // クライアントからのデータを受信し、コールバックで処理
    while (1) {
        len = ipc_recv(sock, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            // データ受信
        	buffer[len] = '\0';
        	printf("[DEBUG] Received %d bytes\n", len);
        	arg->callback(arg->client_sock, buffer, len, arg->user_data);
        } else if (len == 0) {
            // 相手が切断
        	printf("[DEBUG] Client closed connection.\n");
            break;
        } else if (len == -1) {
            // エラー発生
            break;
        } else if (len == -2) {
            // まだデータなし → ループ継続
        }
    }

    // ソケットを閉じる
    ipc_close(sock);
    free(arg);  // スレッド引数の解放

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 * @brief サーバーを起動してクライアント接続を待機し、各接続ごとにスレッドで処理
 *
 * @param ip   待ち受けIPアドレス文字列（例: "127.0.0.1"）
 * @param port 待受ポート番号
 * @param callback データ受信時に呼ばれるコールバック関数
 * @param user_data コールバック関数に渡される任意のユーザーポインタ
 * @return int 成功:0, 失敗:-1
 */
int ipc_server_start(const char *ip, int port, ipc_receive_callback callback, void *user_data) {
    ipc_socket_t server_sock;
    struct sockaddr_in addr;

    // サーバーソケット作成
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == INVALID_SOCKET) {
        perror("socket");
        return -1;
    }

    // バインド情報設定
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);  // 任意のIPで待受
    addr.sin_port = htons(port);        // ポート番号をネットワークバイト順に変換

    // ポート再利用を許可（TIME_WAIT対策）
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));


    // ソケットにバインド
    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("bind");
        ipc_close(server_sock);
        return -1;
    }

    // 接続待ち状態に
    if (listen(server_sock, 5) == SOCKET_ERROR) {
        perror("listen");
        ipc_close(server_sock);
        return -1;
    }

#ifdef _WIN32
    // Windowsでは非ブロッキングにする
    u_long mode = 1;
    ioctlsocket(server_sock, FIONBIO, &mode);
#else
    // Linuxでは非ブロッキングにする
    int flags = fcntl(server_sock, F_GETFL, 0);
    fcntl(server_sock, F_SETFL, flags | O_NONBLOCK);
#endif

    printf("Server listening on %s:%d\n", ip, port);
    ipc_server_running = 1;

    // 終了が呼ばれるまで無限ループ
    while (ipc_server_running) {
        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(server_sock, &readfds);

        tv.tv_sec = 0;
        tv.tv_usec = 500000; // 0.5秒タイムアウトで終了フラグを確認

        int ret = select(server_sock + 1, &readfds, NULL, NULL, &tv);
        if (ret > 0 && FD_ISSET(server_sock, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t addrlen = sizeof(client_addr);
            ipc_socket_t client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addrlen);

            if (client_sock != INVALID_SOCKET) {
                printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

                client_thread_arg_t *arg = malloc(sizeof(client_thread_arg_t));
                arg->client_sock = client_sock;
                arg->callback = callback;
                arg->user_data = user_data;

#ifdef _WIN32
                CreateThread(NULL, 0, client_thread, arg, 0, NULL);
#else
                pthread_t tid;
                pthread_create(&tid, NULL, client_thread, arg);
                pthread_detach(tid);
#endif
            }
        }
        // タイムアウトでここに戻って終了フラグを確認
    }

    ipc_close(server_sock);
    printf("Server stopped.\n");
    return 0;
}

/**
 * @brief サーバーを停止する
 */
void ipc_server_stop(void) {
    ipc_server_running = 0;
}

/**
 * @brief クライアントソケットを作成し、指定IP/ポートに接続する
 *
 * @param ip   接続先IPアドレス文字列（例: "127.0.0.1"）
 * @param port 接続先ポート番号
 * @return 成功: ソケットハンドル, 失敗: INVALID_SOCKET
 */
ipc_socket_t ipc_client_connect(const char *ip, int port) {
    ipc_socket_t s;
    struct sockaddr_in addr;

    // ソケット生成
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        perror("socket");
        return INVALID_SOCKET;
    }

    // 接続先アドレス設定
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    // サーバーに接続
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("connect");
        ipc_close(s);
        return INVALID_SOCKET;
    }

    printf("Connected to %s:%d\n", ip, port);
    return s;
}

/**
 * @brief データを送信する
 *
 * @param sock 通信中のソケット
 * @param data 送信するデータ
 * @param len  送信するデータのサイズ（バイト）
 * @return 送信したバイト数, 失敗時は -1
 */
int ipc_send(ipc_socket_t sock, const void *data, int len) {
#ifdef _WIN32
	// Windowsではキャストが必要（const void* → const char*）
    return send(sock, (const char *)data, len, 0);
#else
    return send(sock, data, len, 0);
#endif
}

/**
 * @brief データを受信する
 *
 * @param sock 通信中のソケット
 * @param buf  受信データを格納するバッファ
 * @param len  バッファサイズ
 * @return 受信したバイト数, 切断またはエラー時は -1
 */
int ipc_recv(ipc_socket_t sock, void *buf, int len) {
    int ret = recv(sock, buf, len, 0);

#ifdef _WIN32
    if (ret == SOCKET_ERROR) {
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            // データがまだ届いていないだけ
            return -2;
        } else {
            printf("[DEBUG] recv() error: %d\n", err);
            return -1;
        }
    }
#else
    if (ret < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // データがまだ届いていないだけ
            return -2;
        } else {
            perror("[DEBUG] recv() error");
            return -1;
        }
    }
#endif

    return ret; // 受信したバイト数 or 0 (切断)
}

/**
 * @brief ソケットを閉じる
 *
 * @param sock 閉じるソケット
 */
void ipc_close(ipc_socket_t sock) {
#ifdef _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
}

