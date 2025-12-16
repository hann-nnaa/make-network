# Portfolio - Network Tool & Mini Flask App

## 概要
九州工業大学 情報通信工学科 3年 プロジェクトサンプル
このプロジェクトは、ネットワークの基本動作を理解するために作った自作の低レベルネットワークツール
・ping.c(ICMP Echo)
・traceroute(TTL制御)
・共通処理(チェックサム計算、時間計測)
・Flaskを使ったWebインターフェイス

### ディレクトリ構成
portfolio/
├─ network_tool/
│ ├─ Makefile # ping / traceroute をビルドする Makefile
│ ├─ ping.c # ICMP Echo を用いた ping 実装
│ ├─ ping # コンパイル後の ping 実行ファイル
│ ├─ traceroute.c # TTL 制御による traceroute 実装
│ ├─ traceroute # コンパイル後の traceroute 実行ファイル
│ ├─ utils.c # 共通処理（チェックサム計算・時間計測）
│ ├─ utils.o # utils.c のオブジェクトファイル
│ └─ common.h # 共通ヘッダファイル
│
└─ web_app/
├─ app.py # Flask アプリケーション本体
├─ requirements.txt# Flask 等の依存ライブラリ一覧
├─ static/
│ └─ style.css # Web画面用CSS
└─ templates/
├─ base.html # 共通レイアウトテンプレート
├─ index.html # IP・ツール選択画面
├─ login.html # ログイン画面
└─ result.html # 実行結果表示画面

## 事前準備
RAWソケットを使うため、Linux(WSL,Ubuntuなど)が必要です

まず依存パッケージをインストール:
sudo apt update
sudo apt install build-essential python3-pip

Flaskのインストール:
pip3 install flask

## ビルド方法
Makefileがあるので、ディレクトリ内で以下を実行します
cd network_tool
make

成功すると以下のファイルができます
・ping
・traceroute

RAWソケット実行権限の設定
通常ユーザーがRAWソケットを利用するにはsetcapが必要です
sudo setcap cap_net_raw+ep ./ping
sudo setcap cap_net_raw+ep ./traceroute
確認：
getcap ./ping
getcap ./traceroute

## 実行方法
### ping
./ping <IPアドレスI


