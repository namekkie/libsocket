/*
 * ipc.h
 *
 *  Created on: 2025/10/16
 *      Author: daiki
 */
#ifndef IPC_H
#define IPC_H

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  typedef SOCKET ipc_socket_t;
  typedef HANDLE ipc_thread_t;
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <pthread.h>
  #include <fcntl.h>
  typedef int ipc_socket_t;
  typedef pthread_t ipc_thread_t;
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR   -1
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// =======================================================
// 型定義
// =======================================================
/**
 * クライアントからデータを受信したときに呼ばれるコールバック関数。
 *
 * @param client_sock クライアントソケット
 * @param data        受信データ
 * @param len         データ長
 * @param user_data   登録時のユーザーデータポインタ
 */
typedef void (*ipc_receive_callback)(ipc_socket_t client_sock, const char *data, int len, void *user_data);


// =======================================================
// API
// =======================================================
/**
 * @brief 初期化処理を行う
 * @note Windows用
 * @return 0 成功, -1 失敗
 */
int ipc_init(void);

/**
 * @brief クリーンアップ処理
 * @note Windows用
 */
void ipc_cleanup(void);

/**
 * @brief サーバーを起動する
 * 	各クライアント接続ごとにスレッドを生成して処理する。
 *
 * @param ip バインドするIPアドレス（例: "0.0.0.0"で全てのインターフェース）
 * @param port 待受ポート
 * @param callback データ受信時のコールバック関数
 * @param user_data コールバックに渡す任意のポインタ
 * @return 成功時 0、失敗時 -1
 */
int ipc_server_start(const char *ip, int port, ipc_receive_callback callback, void *user_data);

/**
 * @brief サーバー停止する
 *
 * 外部から呼び出して安全にサーバーループを終了させる
 */
void ipc_server_stop(void);

/**
 * クライアントとして接続する。
 *
 * @param ip サーバーのIPアドレス（例: "127.0.0.1"）
 * @param port ポート番号
 * @return ソケット、失敗時 INVALID_SOCKET
 */
ipc_socket_t ipc_client_connect(const char *ip, int port);
/**
 * @brief データを送信する
 * @param sock ソケット
 * @param data 送信データ
 * @param len  データ長
 * @return 送信バイト数 or -1
 */
int ipc_send(ipc_socket_t sock, const void *data, int len);

/**
 * @brief データを受信する
 * @param sock ソケット
 * @param buf  受信バッファ
 * @param len  バッファ長
 * @return 受信バイト数 or -1
 */
int ipc_recv(ipc_socket_t sock, void *buf, int len);

/**
 * @brief ソケットを閉じる
 */
void ipc_close(ipc_socket_t sock);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_IPC_H_ */
