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
├─ tool_program/  
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
```bash
sudo apt update  
sudo apt install build-essential python3-pip
```

Flaskのインストール:  
```bash
pip3 install flask
```

## ビルド方法
Makefileがあるので、ディレクトリ内で以下を実行します  
```bash
cd tool_program  
make
```

成功すると以下のファイルができます  
・ping  
・traceroute

RAWソケット実行権限の設定  
通常ユーザーがRAWソケットを利用するにはsetcapが必要です  
```bash
sudo setcap cap_net_raw+ep ./ping
sudo setcap cap_net_raw+ep ./traceroute
```
確認：  
```bash
getcap ./ping  
getcap ./traceroute  
```

## 実行方法
### ping
```bash
sudo ./ping <IPアドレス>
```
実行例：  
PING 8.8.8.8: time=22.71ms  

### traceroute
```bash
sudo ./traceroute <IPアドレス>
``` 
実行例：  
Traceroute to 8.8.8.8 (8.8.8.8), 30 hops max  

 1  172.26.224.1     0.585 ms  
 2  131.206.224.1    12.405 ms   
 3  192.47.1.52      9.242 ms  
 4  150.99.193.221   5.940 ms  
 5  150.99.20.125    19.339 ms  
 6  210.173.184.57   21.107 ms  
 7  108.170.255.229  17.860 ms  
 8  72.14.239.25     17.237 ms  
 9  8.8.8.8          17.183 ms  

## Web インターフェイスの使い方(Flask)
### 起動方法  
```bash
cd web_app
python3 app.py
```
### ブラウザでアクセス
`http://Localhost:5000`  

### 画面内容
**・Target IP**: 送信先IPアドレス  
**・Tool**: ping/traceroute どちらかを選択してRunを押すと、実行結果が表示されます

## 改造ポイント
### pingを「定期実行化」
・1～5秒間隔でICMP Echo  　　
・RTTを配列で保持  
・配信中の遅延の揺らぎを再現  

### flask→websocket化
・HTTPではなくwebsocketを使用  
・RTT取得ごとにpush  
・ブラウザ側で即描画  



大学では情報通信工学を専攻し、ネットワーク遅延や通信経路制御を中心に学んできました。
その知識を活かし、VTuber配信における低遅延通信をテーマに、
配信遅延をリアルタイムで可視化するツールを個人で開発しました。

この開発を通して、配信における数十ミリ秒の遅延が、
コメント反映や視聴体験に大きく影響することを実感しました。

貴社のVTuber事業は、エンターテインメント性だけでなく、
その裏側にある配信基盤・技術力によって支えられていると感じています。

私は、通信技術を土台に、
VTuberと視聴者をつなぐ「リアルタイム体験」を技術面から支えるエンジニアとして、
貴社の配信品質向上に貢献したいと考えています。
