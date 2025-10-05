# 出力ファイル
TARGET = tcp_client

# ソースファイル
SRC = ../../sample/tcp/client/tcp_client.c

# ビルドルール
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lws2_32

# クリーン用
clean:
	del $(TARGET).exe   # Windowsの場合
	# rm -f $(TARGET)   # Linux/macOSの場合