# IPC Communication Library

軽量な **TCP/IP 通信ライブラリ (IPCライブラリ)** です。  
Windows と Linux の両方で動作します。  
シンプルなAPIで、サーバー・クライアント間のデータ送受信を簡単に実装できます。

---

## 🚀 特徴

- Windows / Linux 両対応  
- ノンブロッキング通信対応  
- クライアントごとにスレッドを生成して同時接続を処理  
- コールバック関数で受信データを柔軟に処理  
- C言語でシンプル実装（外部依存なし）

---

## 📁 ファイル構成

```bash
ipc/
├── include/
│   └── ipc.h                 # ライブラリのヘッダファイル
├── lib/
│   ├── ipc.c                 # ライブラリの実装
│   └── Makefile              # ライブラリ用Makefile（libipc.aを作る）
├── sample/
│   ├── server.c              # サーバーのサンプル
│   ├── client.c              # クライアントのサンプル
│   └── Makefile              # サンプル用Makefile（libipc.aをリンク）
└── Makefile                  # ルートMakefile（全体ビルドを統括）
```

---

## ⚙️ ビルド方法

#### 🔹 Linux
```bash
cd ipc
make
```

#### 🔹 Windows (MinGW / Eclipse CDTなど)
`ipc/Makefile` を利用するか、
Eclipseのプロジェクト設定で `ipc.c` を追加してください。