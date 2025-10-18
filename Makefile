# トップディレクトリのパス
TOPDIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

.PHONY: all lib sample clean

all: lib sample

# ライブラリをビルド
lib:
	$(MAKE) -C $(TOPDIR)/lib

# サンプルをビルド
sample:
	$(MAKE) -C $(TOPDIR)/sample

# クリーン
clean:
	$(MAKE) -C $(TOPDIR)/lib clean
	$(MAKE) -C $(TOPDIR)/sample clean