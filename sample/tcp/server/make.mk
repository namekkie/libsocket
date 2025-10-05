# 出力ファイル
TARGET = tcp_server

# ソースファイル
SRC = ../../sample/tcp/server/tcp_server.c

# ビルドルール
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lws2_32

# クリーン用
clean:
	del $(TARGET).exe   # Windowsの場合
	# rm -f $(TARGET)   # Linux/macOSの場合